/**
 * Projekt: IFJ11 kompilator.
 * @file: str.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pre string.c.
 */
/**
 * Tento soubor byl pouzit z ukazkoveho projektu prekladace. Byl upraven.
 */

//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

#ifndef STR_H
#define STR_H

typedef struct
{
  char* str;      // misto pro dany retezec ukonceny znakem '\0'
  int length;     // skutecna delka retezce
  int allocSize;  // velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);
char *strGetStr(string *s);
int strGetLength(string *s);

int strToInt(string *snumber,int *number);
int strInitText(string *s, char *text);
int strAddText(string *s1, char *text);
void strSub(string *s1, string *s2,int beg,int end);
void strConCat(string *s1, string *s2, string *s3);
void readStrToNl(string *var);
void readStrToEOF(string *var);
void readStrToNUMB(string *var,int numb);
void strAddNumb(string *var, int number);
void strSearchParam(string *func, string *ref,int param);
int isEmpty(string *text);

#endif // STR_H
/*** Konec souboru str.h ***/
