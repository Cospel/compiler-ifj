/**
 * Projekt: IFJ11 kompilator.
 * @file: jumpstack.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pro pomocny pseudozasobnik.
 */

#ifndef _HELPSTACK_H
#define _HELPSTACK_H

#define STACK_SIZE 100  // velkost zasobniku
#define S_LAB 1         // udava ze polozka je promenna
#define S_VAR 0         // udava ze polozka je navesti

// Pomocni struktura pro data pri volani funkci. Prvel seznamu.
typedef struct hListItem
{
  int type;                    // typ prvku NAVESTI S_LAB nebo promenna S_VAR
  tData data;
  struct hListItem *nextItem;  // nasledni prvek
  struct hListItem *prevItem;  // predchazejici prvek
} HtItemData;

// Seznam pro ukladani polozek, pri volani funkci.
typedef struct
{
  HtItemData *first;  // ukazatel na prvni prvek
  HtItemData *last;   // ukazatel na posledni prvek
} tListOfCall;

// Zasobnik pro skoky
typedef struct {
  tInstr **jumps;  // adresy na navesti
  int stack_size;  // velikost zasobniku
  int top;         // vrchol zasobniku
} jumpStack;

// Funkce pro jumpStack
void jStackDispose(jumpStack *stack);
void jStackInit(jumpStack *stack);
void jStackPush(jumpStack *stack, tInstr *Instruction);
tInstr *jStackPop(jumpStack *stack);

// Funkce pro hList
void hListClearToLab(tListOfCall *L);
tData *hSearchToLab(tListOfCall *L, tData *data, int PUSHING);
void hListInsert(tListOfCall *L, tData *data, int var);
void hListInit(tListOfCall *L);
void hListDispose(tListOfCall *L);
void printfHList(tListOfCall *L);

#endif
/*** Konec souboru helpStack.h ***/
