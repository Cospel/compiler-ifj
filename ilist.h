/**
 * Projekt: IFJ11 kompilator.
 * @file: ilist.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pre linearny zoznam instrukci.
 */
/**
 * Tento soubor byl pouzit z ukazkoveho projektu prekladace.
 */

#define I_LAB_MAIN_ILIST 0

typedef struct
{
  int instType; // typ instrukce
  void *addr1;  // adresa 1
  void *addr2;  // adresa 2
  void *addr3;  // adresa 3
} tInstr;

typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;

typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
} tListOfInstr;

void printfIlist(tListOfInstr *l);
void listInit(tListOfInstr *L);
void listFree(tListOfInstr *L);
void listInsertLast(tListOfInstr *L, tInstr I);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);
void *listGetPointerLast(tListOfInstr *L);
void listGotoMain(tListOfInstr *L);
tInstr *listGetData(tListOfInstr *L);
