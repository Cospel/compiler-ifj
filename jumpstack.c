/**
 * Projekt: IFJ11 kompilator.
 * @file: jumpstack.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro pomocny pseudozasobnik.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "ial.h"
#include "ilist.h"
#include "jumpstack.h"


///////////////////// ZASOBNIK PRO SKOKY ///////////////////////////////////////
/**
 * Odstran zasobnik.
 * @param stack je zasobnik
 */
void jStackDispose(jumpStack *stack)
{
  stack->stack_size = 0;
  stack->top = -1;
  // uvolnime skoky
  free(stack->jumps);
}

/**
 * Inicializuje zasobnik.
 * @param stack je zasobnik
 */
void jStackInit(jumpStack *stack)
{
  stack->stack_size = STACK_SIZE;
  stack->top = -1;
  stack->jumps = malloc(sizeof(tInstr)*(stack->stack_size));
}

/**
 * Pushne na zasobnik instrukci/adresu navesti.
 * @param stack je zasobnik
 * @param ins je instrukce ktera se pushuje
 */
void jStackPush(jumpStack *stack, tInstr *ins)
{
  stack->top = stack->top + 1;
  // ak je zasobnik plny tak se realokuje
  if(stack->top == stack->stack_size)
  {
    stack->stack_size = stack->stack_size + STACK_SIZE;
    stack->jumps = realloc(stack->jumps,sizeof(tInstr)*stack->stack_size);
  }
  stack->jumps[stack->top] = ins;
}

/**
 * Popne instrukci/adresu navesti zo zasobniku.
 * @param stack je zasobnik.
 * @return je skok ze zasobniku
 */
tInstr *jStackPop(jumpStack *stack)
{
  tInstr *jump = stack->jumps[stack->top];
  stack->top = stack->top -1;

  // vracim skok z vrchu zasobniku
  return jump;
}

///////////////// SEZNAM PRO REKURZY, HLIST ////////////////////////////////////

/**
 * Funkce inicializuje dvousmerne vazany seznam.
 * @param L je seznam
 */
void hListInit(tListOfCall *L)
{
  L->first = NULL;  // kazdy ukazatel na NULL
  L->last = NULL;
}

/**
 * Funkce uvolni seznam.
 * @param L je seznam/
 */
void hListDispose(tListOfCall *L)
{
  // ak je seznam prazdny
  if(L->first !=NULL)
  {
    HtItemData *item;
    // projdi seznamem kym neni NULL
    while(L->first != NULL)
    {
      item = L->first;
      L->first = item->nextItem;
      // ak je prvek seznamu promenna data tak uvolni retezce v data
      if(item->type == S_VAR)
      {
        strFree(&item->data.varKey);
        if(item->data.varType == T_STRING)
          strFree(&item->data.value.sValue);
      }
      // uvolni
      free(item);
      item = NULL;
    }
  }
  // nastav ukazatele na NULL
  L->first = NULL;
  L->last = NULL;
}

/**
 * Funkce prida na konec seznamu novou polozku.
 * @param L je dvousmerny seznam.
 * @param data jsou data pro pridani do nove polozky
 * @param var je typ prvku S_LAB je navesti S_VAR jsou data
 */
void hListInsert(tListOfCall *L, tData *data, int var)
{
  // maloc na novy prvek
  HtItemData *item = malloc(sizeof(HtItemData));
  item->type = var;
  // ak je prvek obsahujici data
  if(var == S_VAR)
  {
    item->type = S_VAR;
    strInitText(&item->data.varKey,data->varKey.str);
    item->data.varType = data->varType;
    item->data.hasValue = data->hasValue;
    if(item->data.varType == T_STRING)
      strInitText(&item->data.value.sValue,data->value.sValue.str);
    else if(item->data.varType == T_NUMBER)
      item->data.value.dValue = data->value.dValue;
    else if(item->data.varType == T_NIL)
      item->data.value.bValue = 0;
    else if(item->data.varType == T_BOOLEAN)
      item->data.value.bValue = data->value.bValue;
    else if(item->data.varType == T_NOTDEF)
      item->data.value.bValue = 0;
  }
  // jinak je to navesti
  else
    item->type = S_LAB;

  if(L->first == NULL)  // jestlize se jedna vubec o prvni prvek
  {
    item->prevItem = NULL;
    item->nextItem = NULL;
    L->first = item;
    L->last = item;
  }
  else  // jinak pridej na konec
  {
    item->nextItem = NULL;
    item->prevItem = L->last;
    L->last->nextItem = item;
    L->last = item;
  }
}

/**
 * Funkce vycisti seznam do nejblizsiho navesti S_LAB.
 * @param L je seznam.
 */
void hListClearToLab(tListOfCall *L)
{
  if(L->last != NULL)
  {
    // uvolni prvni polozku protoze je vzdy S_LAB
    HtItemData *item = L->last;
    L->last = item->prevItem;
    L->last->nextItem = NULL;
    free(item);
    // uvolnuj az kym neprijdes na dalsi S_LAB
    while(L->last != NULL)
    {
      item = L->last;
      // jestlize je S_LAB prerus uvolnovani jinak uvolni retezce
      if(item->type == S_LAB)
        return;
      else
      {
        strFree(&item->data.varKey);
        if(item->data.varType == T_STRING)
          strFree(&item->data.value.sValue);
      }
      // posun prvky seznamu
      L->last = item->prevItem;
      L->last->nextItem = NULL;
      free(item);
    }
  }
}

/**
 * Toto je pomocna funkce pri testovani seznamu.
 * V programu se jinak vubec nepouziva.
 */
void printfHList(tListOfCall *L)
{
  printf("======================\n");
  printf("NA ZASOBNIKU:\n");
  for(HtItemData *item = L->last; item != NULL; item = item->prevItem)
  {
    if(item->type == S_LAB)
      printf("I_LAB %d\n", item->type);
    else
      printf("I_VAR %s %d\n",item->data.varKey.str,item->data.varType);
  }
  printf("======================\n");
}

/**
 * Prohleda seznam do nejblizsiho navesti a hleda polozku data.
 * @param L je seznam.
 * @param data data ktere se hledaji v seznamu.
 */
tData *hSearchToLab(tListOfCall *L, tData *data, int PUSHING)
{
  if(PUSHING)
  {
    HtItemData *item = L->last;
    for(HtItemData *item2 = item; item2 != NULL; item2 = item2->prevItem)
    {
      if(item2->type == S_LAB)
        return NULL;
      if(strcmp(item2->data.varKey.str,data->varKey.str) == 0)
        return &item2->data;
    }
    return NULL;
  }
  else
  {
    for(HtItemData *item = L->last; item != NULL; item = item->prevItem)
    {
      // jestlize si na S_LAB zacni hledat data s danym retezcem do nejblizsiho S_LAB.
      if(item->type == S_LAB)
      {
        for(HtItemData *item2 = item->prevItem; item2 != NULL; item2 = item2->prevItem)
        {
          if(item2->type == S_LAB)
            return NULL;
          if(strcmp(item2->data.varKey.str,data->varKey.str) == 0)
            return &item2->data;
        }
        return NULL;
      }
    }
  }
  return NULL;
}
/*** Konec souboru helpStack.c ***/
