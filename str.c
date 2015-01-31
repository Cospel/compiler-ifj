/**
 * Projekt: IFJ11 kompilator.
 * @file: str.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pre typ string.
 */
/**
 * Tento soubor byl pouzit z ukazkoveho projektu prekladace. Byl upraven a doplnen.
 */

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "str.h"

#define STR_LEN_INC 16
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR   1
#define STR_SUCCESS 0
#define CHAR_NUMB_LENGTH 10

int strInit(string *s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
   free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = s1->length + STR_LEN_INC;
   }

   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
   return s->length;
}

/**
 * Funkce strInitText inicializuje string a prida retezec do stringu.
 * @param s string
 * @param text retezec
 * @return Uspesnost funkce
 */
int strInitText(string *s, char *text)
{
  if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
     return STR_ERROR;
  s->str[0] = '\0';
  s->length = 0;
  s->allocSize = STR_LEN_INC;

  int i = 0;
  while(text[i] != '\0')
  {
    if(strAddChar(s,text[i]) == STR_ERROR)
      return STR_ERROR;
    i++;
  }
  return STR_SUCCESS;
}

/**
 * Funkce strAddText prida retezec do existujiciho stringu.
 * @param s1 string
 * @param text retezec
 * @return uspesnost funkce.
 */
int strAddText(string *s1, char *text)
{
  int i = 0;
  while(text[i] != '\0')
  {
    if(strAddChar(s1,text[i]) == STR_ERROR)
      return STR_ERROR;
    i++;
  }
  return STR_SUCCESS; 
}

/**
 * Funkce stringToInt prekonvertuje string na integer.
 * @param snumber retazec.
 * @param number je odkaz na integer.
 * @return uspesnost funkce.
 */
int strToInt(string *snumber,int *number)
{
  int j = snumber->length;
  int l = j;
  int num = 0;

  for (int i = 0; i < l; i++)
  {
    if((snumber->str[i] >= '0') && (snumber->str[i] <= '9'))
    {
      num = (int)(snumber->str[i]) - 48;  //48 je ascii hodnota nuly
      for(int k = 0; k < j - 1; k++)
        num = num * 10;  // 10 je desitkova soustava

      j = j - 1;
      *number += num;
    }
    else
      return STR_ERROR;
  }
  return STR_SUCCESS;
}

/**
 * Funkce strConCat udela konkatenaci retezcu s1 a s2 do s3.
 * @param s1 1.retezec
 * @param s2 2.retezec
 * @param s3 vyslednej retezec
 */
void strConCat(string *s1, string *s2, string *s3)
{
  for(int i = 0; i < s1->length; i++)
    strAddChar(s3,s1->str[i]);

  for(int i = 0; i < s2->length; i++)
    strAddChar(s3,s2->str[i]);
}

/**
 * Funkce na vysekavani podretezce.
 * @param s1 prohledavany retezec.
 * @param s2 vyseknuty retezec.
 * @param beg Zacatek vysekavani.
 * @param end Konec vysekavani.
 */
void strSub(string *s1, string *s2,int beg,int end)
{
  // Pocatecni testovani na hranicne body.
  if(beg > s1->length && end > s1->length)
    return;
  if(beg < 0 && end < 0 && beg < -s1->length && end < -s1->length)
    return;
  if(beg == 0 && end == 0)
    return;
  else if(beg < 0 && beg == end)
  {
    strAddChar(s2,s1->str[s1->length+beg]);
    return;
  }
  else if(beg == end || (beg > 0 && end > 0 && beg == s1->length) )
  {
    strAddChar(s2,s1->str[beg-1]);
    return;
  }
  // Pokrocile podminky
  if(beg < 0) beg = beg + s1->length + 1;
  if(end < 0) end = end + s1->length + 1;

  // Vyber nejmensi prvek na zacatek
  int beg1 = 0; int end1 = 0;
  if(beg < end)
  {
    beg1 = beg;
    end1 = end;
  }
  else
  {
    beg1 = end;
    end1 = beg;
  }

  // Projdi cyklem a pridej znaky do retezce
  for(int i = beg1; i <= end1; i++)
  {
    if(!(i -1 < 0 || i -1 > s1->length))
      strAddChar(s2,s1->str[i-1]);
  }
}

/**
 * Funkce readStrToNUMB nacita z stdin retezec velkosti numb.
 * @param var string.
 */
void readStrToNUMB(string *var, int numb)
{
  int c;
  for(int i = 0; i < numb; i++)
  {
    c = getchar();
    strAddChar(var,(char)c);
  }
}

/**
 * Funkce readStrToNl nacita z stdin retezec po konec radku.
 * @param var string.
 */
void readStrToNl(string *var)
{
  int c;
  while((c = getchar()) != '\n')
    strAddChar(var,(char)c);
}

/**
 * Funkce readStrToEOF nacita z stdin retezec po EOF.
 * @param var string.
 */
void readStrToEOF(string *var)
{
  int c;
  while((c = getchar()) != -1)// EOF
    strAddChar(var,c);
}

/**
 * Funkcia prida do retezce cislo.
 * @param var retezec kam se cislo prida.
 */
void strAddNumb(string *var, int number)
{
  char arrayForNumbs[CHAR_NUMB_LENGTH];
  snprintf(arrayForNumbs, 10, "%i", number);
  strAddText(var,arrayForNumbs);
}

/**
 * Funkce strSearchParam vyhleda parametr v retezci oddeleny ';'.
 * @param func prohledavany retezec.
 * @param ref odkaz na retezec kam pridam parametr
 * @param param ktory parameter v poradi.
 */
void strSearchParam(string *func, string *ref,int param)
{
  int countParam = 0;
  for(int i = 0; i < func->length; i++)
  {
    if(func->str[i] == ';')
      countParam++;
    if(countParam == param)
    {
      if(func->str[i] != ';')
        strAddChar(ref,func->str[i]);
    }
  }
}

/**
 * Funkce vrati 1 pokud je retezec prazdny inak 0.
 */
int isEmpty(string *text)
{
  for(int i = 0; i < text->length; i++)
    return 0;
  return 1;
}
/*** Konec souboru str.c ***/
