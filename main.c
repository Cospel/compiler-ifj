/**
 * Projekt: IFJ11 kompilator.
 * @file: main.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro hlavni funkci main.
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "str.h"
#include "ilist.h"
#include "ial.h"
#include "scaner.h"
#include "expr.h"
#include "parser.h"
#include "interpret.h"

const char *ECODEMSG[] =
{
  "EOK\n",
  "Chyba v ramci lexikalni analyzy.\n",
  "Chyba v ramci syntakticke analyzy.\n",
  "Chyba v ramci semanticke analyzy.\n",
  "Chyba v ramci interpretace.\n",
  "Interni chyba pri prekladu.\n",
};

/**
 * Funkce vytiskne chybovou hlasku.
 * @param type Typ chyby.
 */
void printError(int type)
{
  fprintf(stderr, "%s", ECODEMSG[type]);
}

/**
 * Hlavni funkce main.
 * @param argc Pocet parametru programu.
 * @param argv Parametre programu.
 */
int main(int argc,char *argv[])
{
  // Otevreme zdrojovy soubor
  FILE *fr = NULL;
  if(argc != 2 || ((fr = fopen(argv[1],"r")) == NULL)) {
    printError(EDEF);
    return EDEF;
  }

  // Nastavme zdrojovy soubor
  setSourceFile(fr);
  int result = EIOK;

  // Inicializace tabulky symbolu
  BNode btree;
  initTree(&btree);

  // Inicializace seznamu instrukci
  tListOfInstr iList;
  listInit(&iList);

  // Volani scanner + parser
  if((result = program(&iList,&btree)) != EIOK)
  {
    printError(result);
    listFree(&iList);
    destroyTree(&btree);
    fclose(fr);
    return result;
  }

  // Volani interpretu
  if((result = interpret(&iList,&btree)) != EIOK)
  {
    printError(result);
    listFree(&iList);
    destroyTree(&btree);
    fclose(fr);
    return result;
  }

  // Uvolneni zdroju
  listFree(&iList);
  destroyTree(&btree);
  fclose(fr);
  return EIOK;
}
/*** Konec souboru main.c ***/
