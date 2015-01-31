/**
 * Projekt: IFJ11 kompilator.
 * @file: expr.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro analyzator vyrazu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "str.h"
#include "ial.h"
#include "ilist.h"
#include "scaner.h"
#include "expr.h"
#include "interpret.h"

BNode *btree;
string attr;
int token;
int uniqueKey;
int uniqueKeyNumber;
int paranthese=0;

int STACK_SIZE1 = MAX_STACK;

tListOfInstr *list;

const char *ERRESERVED[] =
{
  "do",
  "else",
  "end",
  "false",
  "function",
  "if",
  "local",
  "nil",
  "read",
  "return",
  "than",
  "true",
  "while",
  "write",
  "and",
  "break",
  "elseif",
  "for",
  "in",
  "not",
  "or",
  "repeat",
  "until",
  "main",
  "sort",
  "type",
  "substr",
  "find",
};

int PaTable [19][19] = {
/*      	0 +     1 -     2 *     3 /     4 ^     5 ..    6 ==    7 ~=    8 <     9 >     10 <=   11 >=   12 f    13 i    14 (    15 )    16 ,   	17 $    18 %    */
/*0 +   */{	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,   PaLs,	},
/*1 -   */{	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*2 *   */{	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,   },
/*3 /   */{	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,   },
/*4 ^   */{	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,   },
/*5 ..  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*6 ==  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*7 ~=  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*8 <   */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*9 >   */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*10 <= */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*11 >= */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaLs,   },
/*12 f  */{	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEm,	PaEq,	PaEm,	PaEm,	PaEm,	PaEm,   },
/*13 i  */{	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaEm,	PaEm,	PaEm,	PaGr,	PaGr,	PaGr,	PaGr,   },
/*14 (  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaEq,	PaEq,	PaEm,	PaLs,   },
/*15 )  */{	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaEm,	PaEm,	PaEm,	PaGr,	PaGr,	PaGr,	PaGr,   },
/*16 ,  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaEq,	PaEq,	PaEm,	PaLs,   },
/*17 $  */{	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaLs,	PaEm,	PaEm,	PaEm,	PaLs,   },
/*18 %  */{	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaGr,	PaLs,	PaLs,	PaLs,	PaGr,	PaGr,	PaGr,	PaGr,   },
};

/*Pripojeni stromu k pouziti pro SA výrazu*/
void nastavStrom(BNode *node)
{
  btree=node;
}

void exprStackInit (tStack * s)
{
  s->top = -1;
  s->block = STACK_SIZE1;
  s->arr = malloc(sizeof(tElement)*(STACK_SIZE1));
}

int exprStackEmpty (const tStack * s)
{
  return (s->top == -1 ? 1 : 0);
}

int exprStackFull (const tStack * s)
{
  return ((s->top) == s->block - 1 ? 1 : 0);
}

void exprStackTopTerminal (const tStack * s, tElement * element)
{
  element->type = EEXPR;
  int i = 0;
  while (element->type > ESTRING)
  {
    *element = s->arr[s->top + i];
    i--;
  }
}

void exprStackTopAll (const tStack * s, tElement * element)
{
  *element = s->arr[s->top];
}

void exprStackPop (tStack * s)
{
  s->top--;
}

void exprStackPush (tStack * s, tElement * element)
{
  s->top++;
  if (exprStackFull(s))
  {
    s->block = s->block + STACK_SIZE1;
    s->arr = realloc(s->arr,sizeof(tElement)*s->block);
  }
  s->arr[s->top] = *element;
}

void exprStackDispose(tStack *s)
{
  s->block = 0;
  s->top = -1;
  // uvolnime skoky
  free(s->arr);
}



int nextToken (string * attr, int *func)
{
  int type;
  if((type = getNextToken(attr)) == ELEX) return EERROR;
    switch (type)
    {
      case PLUS:
        type = EPLUS;                // +
        break;
      case MINUS:
        type = EMINUS;               // -
        break;
      case KRAT:
        type = EMUL;                 // *
        break;
      case DELENO:
        type = EDIV;                 // /
        break;
      case MODULO:
        type = EMOD;                 // %
        break;
      case MOCNINA:
        type = EPOW;                 // ^
        break;
      case KONKATENACE:
        type = ECONCAT;              // ..
        break;
      case POROVNANI:
        type = EEQUAL;               // ==
        break;
      case NEROVNASE:
        type = ENEQUAL;              // ~=
        break;
      case MENSI:
        type = ELESS;                // <
        break;
      case VETSI:
        type = EGREAT;               // >
        break;
      case MENSIROVNO:
        type = ELEQUAL;              // <=
        break;
      case VETSIROVNO:
        type = EGEQUAL;              // >=
        break;
      case FUNCTION:
        type = EFUNC;                // f
        break;
      case ID:
        type = EID;                  // i
        break;
      case CISLO:
        type = ENUMBER;              // cislo
        break;
      case TEXT_RETEZEC:
        type = ESTRING;              // string
        break;
      case NIL:
        type = ENIL;                 // NIL
        break;
      case LEVA_NORMAL_ZAV:
        type = ELPARA;               // (
        if(*func==RVWrite)
          paranthese++;
        break;
      case PRAVA_NORMAL_ZAV:         // )
        if((*func==RVWrite) && (paranthese==0)){
          type=ELAST;
          *func=RVRPara;
        }
        else if(*func==RVWrite){
          paranthese--;
          type = ERPARA;
        }
        else
          type = ERPARA;
        break;
      case CARKA:                    // ,
        if(*func==RVWrite){
          type=ELAST;
          *func=RVComma;
        }
        else
          type = ECOMMA;
        break;
      case STREDNIK:
        if(*func==RVWhile || *func==RVIf)  // Pokud prijde WHILE nebo IF a prijde strednik
          type = EERRORSYN;
        else
          type = ELAST;              // ;
        break;
      case DO:                       // Pokud prijde While cekam DO
        if(*func==RVWhile)
          type = ELAST;
        else
          type = EERRORSYN;          // Jinak je DO chyba
        break;
      case THEN:                     // Pokud prijde IF cekam THEN
        if(*func==RVIf)
          type = ELAST;
        else
          type = EERRORSYN;          // Jinak je THEN chyba
        break;
      case READ:
        type = EREAD;                //read
        break;
      case WRITE:
        type = EWRITE;               //write
        break;
      case TRUE:
        type = EBOOL;               //write
        break;
      case FALSE:
        type = EBOOL;               //write
        break;
      default:
        type = EERRORSYN;
        break;
    }
    return type;
}

string genUniqueKey (string funcName)
{
//Vytvoreni unikatniho identifikatoru pro mezivysledky
  string dollar;
  string result;
  strInitText (&dollar, "$");
  strInitText (&result, funcName.str);

  int i;
  i = uniqueKey;
  while (i != 0)
  {
    strAddChar(&dollar, (char)(i % 10 + '0'));
    i = i / 10;
  }

  strAddText (&result, dollar.str);
  strFree (&dollar);
  ++uniqueKey;
  return result;
}

string genUniqueKeyNumber (void)
{
//Vytvoreni unikatniho identifikatoru pro mezivysledky
  string dol_char;
  strInitText (&dol_char, "$");

  int i;
  i = uniqueKeyNumber;
  while (i != 0)
  {
    strAddChar(&dol_char, (char)(i % 10 + '0'));
    i = i / 10;
  }

  ++uniqueKeyNumber;
  return dol_char;
}

void generateInstruction (int instType, void *addr1, void *addr2, void *addr3)
// vlozi novou instrukci do seznamu instrukci
{
  tInstr I;
  I.instType = instType;
  I.addr1 = addr1;
  I.addr2 = addr2;
  I.addr3 = addr3;
  listInsertLast (list, I);
}

void deallocElements(tElement *token, tElement *tmp, tElement *var1, tElement *var2, tStack *stack, string *varName)
{
//dealokuje vsechny dynamicke prvky
  exprStackDispose(stack);
  free (stack);
  free (token);
  free (tmp);
  free (var1);
  free (var2);
  strFree (varName);
}

/*
func-parametr, ktery oznacuje vyjimecne stavy(napr. write, read, fci
funcName-nazev funkce ve vstupnim souboru, ve ktere syntAnalExpr pracuje
iList2-seznam instrukci
*/
tElement *syntAnalExpr (int *func, string funcName, tListOfInstr * iList2)
{
  list = iList2;

  tStack *stack;                // Struktura pro zasobnik
  stack = (tStack *) malloc (sizeof (tStack));
  exprStackInit (stack);        // Inicializace zasobniku

  tElement *token;              // Pomocna struktura pro prijimani tokenu
  if ((token = (tElement *) malloc (sizeof (tElement)))==NULL){      //pro nacteni tokenu
    token->error=EDEF;
    return token;
  }

  tElement *tmp;                // Pomocna struktura
  if ((tmp = (tElement *) malloc (sizeof (tElement)))==NULL){        //pro vyhledavani na zasobniku
    tmp->error=EDEF;
    return tmp;
  }
  tmp->type = ELAST;               // Prvni znak jdouci na zasobnik

  tElement *getResult;
  if ((getResult = (tElement *) malloc (sizeof (tElement)))==NULL){ //pro vkladani vysledku
    getResult->error=EDEF;
    return getResult;
  }
  getResult->type = EEXPR;
  getResult->error = EIOK;
  // Zpracovany identifikator

  tElement *var1;               // Struktura pro ulozeni prvni hodnoty vyrazu
  if ((var1 = (tElement *) malloc (sizeof (tElement)))==NULL){       //pro vkladani vysledku
    var1->error=EDEF;
    return var1;
  }
  var1->type = EEXPR;              // Zpracovany identifikator
  var1->error = EIOK;

  tElement *var2;               // Struktura pro ulozeni druhe hodnoty vyrazu
  if ((var2 = (tElement *) malloc (sizeof (tElement)))==NULL){       //pro vkladani vysledku
    var2->error=EDEF;
    return var2;
  }
  var2->type = EEXPR;              // Zpracovany identifikator
  var2->error = EIOK;

  int get_token = 1;            // Zda-li se ma ci nema prijimat token


  exprStackPush (stack, tmp);   // Na zasobnik vlozim jako prvni $

  tData aa;                     // Struktura, ktera se vklada to stromu

  string varName;               // String pro pojmenovavani
  string pomAttr;               // Pomocny string pro pouziti u pomocnych struktur
  string dollar;                // Pomocny string pro vkladani $
  double number = 0;            // Zpracovani ciselnych vstupnich hodnot
  strInit (&varName);

  token->data=NULL;
  int notCompared=0;

//Cyklus nacitani znaku a jejich zpracovani pomoci zasobniku a LR tabulky
  while (1)
  {
/*Vyrazdnim obsah token, nebot v nem mohou zustat atributy z predchozich cyklu
pro pocitani potrebuji mit u neidentifikatoru type I_NOTDEF*/
      //token->data->varType=T_NOTDEF;
      if (get_token == 1) token->type = nextToken (&attr, func);  //Nacteni tokenu
      if (token->type == EERROR){
        getResult->error=ELEX;
        deallocElements(token, tmp, var1, var2, stack, &varName);
        return getResult;
      }
      else if(token->type == EERRORSYN){
        getResult->error=ESYN;
        deallocElements(token, tmp, var1, var2, stack, &varName);
        return getResult;
      }

// Pokud prijde jako prvni znak na zasobnik konec vyrazu( );,  ), nebo pokud prijde neocekavany konec souboru
        if(((stack->top==0) && (token->type==ELAST)) || (token->type==EERROR))
        {
          getResult->error=ESYN;
          if(*func==RVRPara)
            getResult->error=EIOK;
          deallocElements(token, tmp, var1, var2, stack, &varName);
          return getResult;
        }

// Vycisteni varName z predchozich cyklu a pridani jmena funkce, ve ktere byl syntAnalExpr zavolan
      strClear (&varName);
      strCopyString (&varName, &funcName);

/*Zpracovani nekterych vyznamnych typu pri jejich nacteni, zahrnuje i ukonceni
v nekterych nestandartnich situacich*/
      if (token->type == EID)            // Zpracovani identifikatoru
      {
        strAddText (&varName, "_");
        strAddText (&varName, attr.str);
        token->data = searchBtreeStr (btree, &varName);
        token->lit = T_ID;
        if (token->data == NULL)
        {
          strInitText(&aa.varKey,attr.str);
          int resWord=0;
          for(int i=ERDO;i<=ERFIND;i++){
            if (strcmp(attr.str, ERRESERVED[i]) == 0)
              resWord=1;
          }

          token->type = nextToken (&attr, func);
          if (token->type == EERROR){
            getResult->error=ELEX;
            deallocElements(token, tmp, var1, var2, stack, &varName);
            return getResult;
          }
          else if (token->type == EERRORSYN){
            getResult->error=ESYN;
            deallocElements(token, tmp, var1, var2, stack, &varName);
            return getResult;
          }

/*Pokud prijmu funkci predam SA pomoci parametru tuto informaci a dealokuji
vsechnu dynamicky pridelenou pamet. Pokud prijmu obycejny obycejny identifikator,
ktery ovsem nenaleznu v tabulce koncim se semantickou chybou*/
          else if(token->type == ELPARA)
          {
            aa.varType = T_FUNC;
            aa.hasValue = HAVENT_VALUE;
            if ((getResult->data = malloc(sizeof(tData)))==NULL){
              getResult->error=EDEF;
              return getResult;
            }
            strInitText(&getResult->data->varKey, aa.varKey.str);
            *func=RVFunc;
            strFree(&aa.varKey);
          }
          else
          {
            if (resWord == 0)
              getResult->error=ESEM;
            else
              getResult->error=ESYN;
          }
          deallocElements(token, tmp, var1, var2, stack, &varName);
          return getResult;
        }
      }
      else if (token->type == ESTRING)       // Zpracovani stringu
      {
          dollar = genUniqueKeyNumber ();
          token->data = searchBtreeStr (btree, &dollar);
          if (token->data == NULL)
          {
              aa.varKey = dollar;
              aa.varType = T_STRING;
              aa.hasValue = HAVE_VALUE;
              strInit (&aa.value.sValue);
              strAddText (&aa.value.sValue, attr.str);
              token->data = getInsertBtree (btree, &dollar, &aa);
              strFree (&aa.value.sValue);
          }
          strFree(&dollar);
          token->lit=T_STRING;
          token->type = EID;
      }
      else if (token->type == ENUMBER)       // Zpracovani cisla
      {

          dollar = genUniqueKeyNumber ();
          token->data = searchBtreeStr (btree, &dollar);
          if (token->data == NULL)
          {
              aa.varKey = dollar;
              aa.varType = T_NUMBER;
              aa.hasValue = HAVE_VALUE;
              number = atof (attr.str);
              aa.value.dValue = number;
              token->data = getInsertBtree (btree, &dollar, &aa);
          }
          token->type = EID;
          token->lit=T_NUMBER;
          strFree (&dollar);
      }
      else if (token->type == ENIL)       // Zpracovani NIL
      {
          dollar = genUniqueKeyNumber ();
          token->data = searchBtreeStr (btree, &dollar);
          if (token->data == NULL)
          {
              aa.varKey = dollar;
              aa.varType = T_NIL;
              aa.hasValue = HAVE_VALUE;
              aa.value.bValue = 0;
              token->data = getInsertBtree (btree, &dollar, &aa);
          }
          token->type = EID;
          token->lit=T_NIL;
          strFree (&dollar);
      }
      else if (token->type == EBOOL)       // Zpracovani bool
      {
          dollar = genUniqueKeyNumber ();
          token->data = searchBtreeStr (btree, &dollar);
          if (token->data == NULL)
          {
              aa.varKey = dollar;
              aa.varType = T_BOOLEAN;
              aa.hasValue = HAVE_VALUE;
              if (strcmp(attr.str,"true")==0)
                aa.value.bValue = 1;
              else
                aa.value.bValue = 0;
              token->data = getInsertBtree (btree, &dollar, &aa);
          }
          token->type = EID;
          token->lit=T_BOOLEAN;
          strFree (&dollar);
      }
/*V pripade ze prijmu funkci read, preadam tuto informaci SA pomoci parametru*/
      else if (token->type == EREAD)       // Zpracovavam read
      {
          *func = RVRead;
          getResult->error = EIOK;
          deallocElements(token, tmp, var1, var2, stack, &varName);
          return getResult;
      }

      exprStackTopTerminal (stack, tmp);
/*Cast starajici se o radne ukonceni programu*/
      if ((tmp->type == ELAST) && (token->type == ELAST))
      {
          exprStackTopAll (stack, getResult);
          if(getResult->data==NULL) // Pokud neni na zasobniku
            getResult->error=ESYN;
          else
            getResult->data->prmCount = 0;

          deallocElements(token, tmp, var1, var2, stack, &varName); // Dealokuji

          if((*func==RVComma) || (*func==RVRPara))          //funkce write
            generateInstruction (I_WRITE, getResult->data, NULL, NULL);
          else if (((*func==RVIf) || (*func==RVWhile)) && (notCompared==1)){      // Zpracovani podminky cyklu
            dollar = genUniqueKeyNumber ();
            aa.varKey = dollar;
            aa.varType = getResult->data->varType;
//nejspíš bude leakovat
            getResult->data = getInsertBtree (btree, &dollar, &aa);
            strFree (&dollar);
            generateInstruction (I_STRUE, getResult->data, NULL, NULL);
            getResult->data->prmCount = 0;
          }
          else
            *func = RVNull;
          return getResult;
      }
// Zpracovani pravidel z LR tabulky
      switch (PaTable[tmp->type][token->type])
      {
        case PaLs:
          exprStackPush (stack, token);
          get_token = 1;
          break;
        case PaEq:
          exprStackPush (stack, token);
          get_token = 1;
          break;
        case PaGr:
          exprStackTopAll (stack, tmp);	//budu hledat pravidla, proto potřebuji vrchol zasobniku
          switch (tmp->type)
          {
            case EID:            // E->i redukce identifikatoru
              exprStackPop (stack);
              getResult->data = tmp->data;
              getResult->lit = tmp->lit;
              exprStackPush (stack, getResult);
              break;
            case EEXPR:            //ExE matematicka operace
              exprStackPop (stack);
              var2->data = tmp->data;	// Hodnota druheho identifikatoru operace ExE
              var2->lit = tmp->lit;
              exprStackTopAll (stack, tmp);
              /*if ((var2->data->varType == T_STRING) && (tmp->type < ECONCAT)) // Osetreni pro scitani,odcitani,... retezcu
              {
                  getResult->error = ESYN;
                  deallocElements(token, tmp, var1, var2, stack, &varName);
                  revim turn getResult;
              }*/
              switch (tmp->type)
              {
                case EPLUS:         // Operace scitani
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;       // Hodnota prvniho identifikatoru
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;

                  getResult->data = getInsertBtree (btree, &pomAttr, &aa); // Pridani struktury do stromu

                  generateInstruction (I_ADD, getResult->data, var1->data, var2->data); // Generovani instrukce
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EMINUS:         // Operace odcitani
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;

                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_SUB, getResult->data, var1->data, var2->data);
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EMUL:         // Operace nasobeni
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;

                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_MUL, getResult->data, var1->data, var2->data);
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EDIV:         // Operace deleni
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_DIV, getResult->data, var1->data, var2->data);
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EMOD:         // Operace modulo
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_MOD, getResult->data, var1->data, var2->data);
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EPOW:         // Operace mocniny
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type!=EEXPR)
                  {
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  if ((var2->lit!=T_NUMBER && var2->lit!=T_ID) || (tmp->lit!=T_NUMBER && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  aa.varType = T_NUMBER;
                  aa.hasValue = HAVE_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_POWER, getResult->data, var1->data, var2->data);
                  getResult->lit=T_NUMBER;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case ECONCAT:         // Operace konkatenace
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if ((var2->lit!=T_STRING || tmp->lit!=T_STRING) && (var2->lit!=T_ID && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if ((var1->data->varType == T_STRING) && (var2->data->varType == T_STRING))
                      aa.varType = T_STRING;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_CONCAT, getResult->data, var1->data, var2->data);
                  getResult->lit=T_STRING;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EEQUAL:         // Operace rovnosti
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  var1->data = tmp->data;
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_EQUAL, getResult->data, var1->data, var2->data);
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case ENEQUAL:         // Operace neni rovno
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  var1->data = tmp->data;	//hodnota prvniho identifikatoru
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_NEQUAL, getResult->data, var1->data, var2->data);
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case ELESS:         // Operace mensi nez
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (( (var2->lit!=T_NUMBER || tmp->lit!=T_NUMBER) && (var2->lit!=T_STRING || tmp->lit!=T_STRING) ) && (var2->lit!=T_ID && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_LCMP, getResult->data, var1->data, var2->data);
                  getResult->lit=T_BOOLEAN;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EGREAT:         // Operace vetsi nez
                  exprStackPop (stack);
                  exprStackTopAll (stack,tmp);
                  if (( (var2->lit!=T_NUMBER || tmp->lit!=T_NUMBER) && (var2->lit!=T_STRING || tmp->lit!=T_STRING) ) && (var2->lit!=T_ID && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;	//hodnota prvniho identifikatoru
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_GCMP, getResult->data, var1->data, var2->data);
                  getResult->lit=T_BOOLEAN;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case ELEQUAL:         // Operace mensi nebo rovno
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (( (var2->lit!=T_NUMBER || tmp->lit!=T_NUMBER) && (var2->lit!=T_STRING || tmp->lit!=T_STRING) ) && (var2->lit!=T_ID && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;	//hodnota prvniho identifikatoru
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_ELCMP, getResult->data, var1->data, var2->data);
                  getResult->lit=T_BOOLEAN;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                case EGEQUAL:         // Operace vetsi nebo rovno
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (( (var2->lit!=T_NUMBER || tmp->lit!=T_NUMBER) && (var2->lit!=T_STRING || tmp->lit!=T_STRING) ) && (var2->lit!=T_ID && tmp->lit!=T_ID))
                  {
                      getResult->error = ESEM;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
                  }
                  var1->data = tmp->data;	//hodnota prvniho identifikatoru
                  exprStackPop (stack);
                  notCompared=0;

                  pomAttr = genUniqueKey (funcName);
                  aa.varKey = pomAttr;
                  if (var1->data->varType == var2->data->varType)
                      aa.varType = T_BOOLEAN;
                  else
                      aa.varType = T_NOTDEF;
                  aa.hasValue = HAVENT_VALUE;
                  getResult->data = getInsertBtree (btree, &pomAttr, &aa);

                  generateInstruction (I_EGCMP, getResult->data, var1->data, var2->data);
                  getResult->lit=T_BOOLEAN;
                  exprStackPush (stack, getResult);
                  strFree(&pomAttr);
                  break;
                default:
                  getResult->error = ESYN;
                  deallocElements(token, tmp, var1, var2, stack, &varName);
                  return getResult;
                  break;
                }
              break;
            case ERPARA:    // Na vrcholu zasobniku je zavorka
              exprStackPop (stack);
              exprStackTopAll (stack, getResult);
              var1->data = getResult->data;
              var1->lit = getResult->lit;
              if (getResult->type == EEXPR)        // Kdyz je pred zavorkou zpracovany identifikator
                {
                  exprStackPop (stack);
                  exprStackTopAll (stack, tmp);
                  if (tmp->type == ELPARA)           // Kdyz je pred zpracovanym identifikatorem zavorka
                    {
                      exprStackPop (stack);
                      exprStackTopAll (stack, tmp);
                      exprStackPush (stack, var1);      // (E) - vyraz v zavorce
                    }
                  else
                  {     // Pokud pred zpracovanym identifikatorem je neco jineho nez zavorka
                      getResult->error = ESYN;
                      deallocElements(token, tmp, var1, var2, stack, &varName);
                      return getResult;
          }
              }
              else
              {         // Pokud je pred zavorkou neco jineho nez identifikator = syntakticka chyba
                  getResult->error = ESYN;
                  deallocElements(token, tmp, var1, var2, stack, &varName);
                  return getResult;
              }
              break;
            default:    // Pokud je pri redukci na zasobniku nelegalni znak = syntakticka chyba
              getResult->error = ESYN;
              deallocElements(token, tmp, var1, var2, stack, &varName);
              return getResult;
          }
          get_token = 0;
          break;
        case PaEm:      // Pokud narazim na prazdny radek v tabulce, jedna se o syntaktickou chybu
          getResult->error = ESYN;
          deallocElements(token, tmp, var1, var2, stack, &varName);
          return getResult;
          break;
        }
  }
}
