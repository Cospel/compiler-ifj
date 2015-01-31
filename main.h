/**
 * Projekt: IFJ11 kompilator.
 * @file: main.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pre main.c.
 */

#ifndef MAIN_H
#define MAIN_H

/** Kody chyb programu. */
enum tecodes
{
  EIOK = 0,  /**< Vsechno je v poradku.              */
  ELEX = 1,  /**< Chyba v ramci lexikalni analyzy.   */
  ESYN = 2,  /**< Chyba v ramci syntakticke analyzy. */
  ESEM = 3,  /**< Chyba v ramci semanticke analyzy.  */
  EINT = 4,  /**< Chyba v ramci interpretace.        */
  EDEF = 5,  /**< Chyba interni pri prekladu.        */
};

/** Funkcie. */
void printError(int type);
#endif /* MAIN_H */
/*** Koniec souboru main.h. ***/
