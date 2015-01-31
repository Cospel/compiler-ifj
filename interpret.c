/**
 * Projekt: IFJ11 kompilator.
 * @file: interpret.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pre interpretaci.
 */

#include <stdio.h>
#include <string.h>
#include "str.h"
#include "ial.h"
#include "ilist.h"
#include "interpret.h"
#include "main.h"
#include <math.h>
#include "jumpstack.h"

// globalni ukazatele pro uvolneni
BNode *g_btree;
tData *g_retData;
jumpStack *g_jStack;
tListOfCall *g_list;

/**
 * Funkce interpret projde a vykona zoznam trojadresnyho kodu.
 * @param iList Seznam triadresneho kodu.
 * @param btree Binarni strom reprezentujici tabulku symbolu.
 * @return Uspesnost funkce.
 */
int interpret(tListOfInstr * iList, BNode *btree)
{
  // chod na MAIN navesti v seznamu instrukci
  listFirst(iList);
  listGotoMain(iList);
  tInstr *I = listGetData(iList);
  g_btree = btree;

  int PUSHING = 0;

  // tData pre return
  tData retData;
  retData.varType = T_NOTDEF;
  retData.hasValue = 0;
  g_retData = &retData;

  // tData pre push
  tData pushData;
  pushData.varType = T_NOTDEF;
  pushData.hasValue = 0;

  // Data pro 3 adresy instrukci
  tData *data1 = NULL;
  tData *data2 = NULL;
  tData *data3 = NULL;
  tData *var = NULL;   //pomocni pri prohladavani
  tData dataC ;        //pomocni pri vyrazech a = a .. b; ....

  // Pomocny zasobnik pro skoky
  jumpStack jStack;
  jStackInit(&jStack);
  g_jStack = &jStack;

  // Pomocny dvousmerny seznam pro funkce
  tListOfCall list2;
  hListInit(&list2);
  g_list = &list2;
  hListInsert(&list2,NULL,S_LAB);

  // Sme na main navesti ?
  if(I->instType != I_LAB_MAIN)
  {
    errorFree();
    return EINT;
  }

  // Projdi a vykonej triadresny kod
  while (1)
  {
    // Ziskej instrukci ze seznamu
    I = listGetData(iList);

    // Prirad adresy do premennych
    data1 = I->addr1;
    data2 = I->addr2;
    data3 = I->addr3;

    // pokus se vyhledat polozky ze stromu v pomocnej strukture
    if(data1 != NULL && I->instType != I_JUMP && I->instType != I_JMPN)
    {
      var = hSearchToLab(&list2, data1, PUSHING);
      if(var != NULL)
        data1 = var;
    }
    if(data2 != NULL)
    {
      var = hSearchToLab(&list2, data2, PUSHING);
      if(var != NULL)
        data2 = var;
    }
    if(data3 != NULL && I->instType != I_JUMP)
    {
      var = hSearchToLab(&list2, data3, PUSHING);
      if(var != NULL)
        data3 = var;
    }

    // Podle typu instrukce vykonej operaci ////////////////////////////////////
    switch (I->instType)
    {
      // NAVESTI MAIN
      case I_LAB_MAIN:
        PUSHING = 1;
        break;

      // NAVESTI
      case I_LAB:
        break;

      // NAVESTI ZACATEK FUNKCE
      case I_LAB_S:
        PUSHING = 1;
        break;

      // NAVESTI KONEC PUSH
      case I_LAB_F:
        PUSHING = 0;
        hListInsert(&list2,NULL,S_LAB);
        break;

      // SOUCET
      case I_ADD:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = data2->value.dValue + data3->value.dValue;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // MODULO
      case I_MOD:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            if(data3->value.dValue == 0)
            {
              errorFree();
              return EINT;
            }
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = ((int)data2->value.dValue) % ((int)data3->value.dValue);
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // ODECITANI
      case I_SUB:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = data2->value.dValue - data3->value.dValue;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // NASOBENI
      case I_MUL:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = data2->value.dValue * data3->value.dValue;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // DELENI
      case I_DIV:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            if(data3->value.dValue == 0)
            {
              errorFree();
              return EINT;
            }
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = data2->value.dValue / data3->value.dValue;
          }
          data1->value.dValue = data2->value.dValue / data3->value.dValue;
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // KONKATENACE
      case I_CONCAT:
        if(data2->varType == T_STRING && data3->varType == T_STRING)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->varType = T_STRING;
            data1->hasValue = 1;
            strInit(&data1->value.sValue);
            strConCat(&data2->value.sValue, &data3->value.sValue, &data1->value.sValue);
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // KONKATENACE
      case I_SLENGTH:
        if(data3->varType == T_STRING)
        {
          int a = data3->value.sValue.length;
          if(data1->varType == T_STRING)
            strFree(&data1->value.sValue);
          data1->hasValue = HAVE_VALUE;
          data1->varType = T_NUMBER;
          data1->value.dValue = a;
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // UMOCNENI
      case I_POWER:
        if(data2->varType == T_NUMBER && data3->varType == T_NUMBER)
        { 
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->varType = T_NUMBER;
            data1->hasValue = 1;
            data1->value.dValue = 1;
            data1->value.dValue = pow( data2->value.dValue, data3->value.dValue);
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // POROVNANI
      case I_EQUAL:
        data1->varType = T_BOOLEAN;
        data1->hasValue = 1;
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            switch(data2->varType)
            {
              case T_NUMBER:
                if (data2->value.dValue == data3->value.dValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_STRING:
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) == 0)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_BOOLEAN:
                if (data2->value.bValue == data3->value.bValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_NIL:
                data1->value.bValue = 1;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          data1->value.bValue = 0;
          data1->varType = T_BOOLEAN;
        }
        break;

      // NEGACE POROVNANI
      case I_NEQUAL:
        data1->varType = T_BOOLEAN;
        data1->hasValue = 1;
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            switch(data2->varType)
            {
              case T_NUMBER:
                if (data2->value.dValue == data3->value.dValue)
                  data1->value.bValue = 0;
                else
                  data1->value.bValue = 1;
                break;
              case T_STRING:
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) == 0)
                  data1->value.bValue = 0;
                else
                  data1->value.bValue = 1;
                break;
              case T_BOOLEAN:
                if (data2->value.bValue == data3->value.bValue)
                  data1->value.bValue = 0;
                else
                  data1->value.bValue = 1;
                break;
              case T_NIL:
                data1->value.bValue = 0;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          data1->value.bValue = 0;
          data1->varType = T_BOOLEAN;
        }
        break;

      // MENSI NEZ
      case I_LCMP:
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            switch (data2->varType)
            {
              case T_NUMBER:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (data2->value.dValue < data3->value.dValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_STRING:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) < 0)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // VETSI NEZ
      case I_GCMP:
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            data1->hasValue = HAVE_VALUE;
            switch (data2->varType)
            {
              case T_NUMBER:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (data2->value.dValue > data3->value.dValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_STRING:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) > 0)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // MENSI ROVNO
      case I_ELCMP:
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            switch(data2->varType)
            {
              case T_NUMBER:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (data2->value.dValue <= data3->value.dValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_STRING:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) <= 0)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // VETSI ROVNO
      case I_EGCMP:
        if(data2->varType == data3->varType)
        {
          if(data2->hasValue != 0 && data3->hasValue != 0)
          {
            switch (data2->varType)
            {
              case T_NUMBER:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (data2->value.dValue >= data3->value.dValue)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              case T_STRING:
                data1->varType = T_BOOLEAN;
                data1->hasValue = 1;
                if (strcmp(data2->value.sValue.str, data3->value.sValue.str) >= 0)
                  data1->value.bValue = 1;
                else
                  data1->value.bValue = 0;
                break;
              default:
                errorFree();
                return EINT;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // JE VYRAZ TRUE ?
      case I_STRUE:
        if(data1->varType == T_STRING)
        {
          strFree(&data1->value.sValue);
          data1->value.bValue = 1;
        }
        else if(data1->varType == T_BOOLEAN)
        {
          if(data1->value.bValue == 1)
            data1->value.bValue = 1;
          else
            data1->value.bValue = 0;
        }
        else if(data1->varType == T_NIL || data1->varType == T_NOTDEF)
          data1->value.bValue = 0;
        else
          data1->value.bValue = 1;
        data1->varType = T_BOOLEAN;
        break;

      // SKOK POKIAL FALSE
      case I_JMPN:
        if(data3->varType == T_BOOLEAN)
        {
          if(data3->value.bValue == 0)
            listGoto(iList, I->addr1);
        }
        break;

      // SKOK
      case I_JUMP:
        if(I->addr3 != NULL)
          jStackPush(&jStack,I);
        listGoto(iList, I->addr1);
        break;

      // PRIRAZENI
      case I_ASGN:
        if(data3->hasValue != 0)
        {
          dataC.varType = data3->varType;
          data1->hasValue = 1;
          makeCopy(data3, &dataC);
          if(data1->varType == T_STRING)
            strFree(&data1->value.sValue);
          data1->varType = data3->varType;
          switch (dataC.varType)
          {
            case T_NUMBER:
              data1->value.dValue = dataC.value.dValue;
              break;
            case T_STRING:
              strInitText(&data1->value.sValue, dataC.value.sValue.str);
              break;
            case T_BOOLEAN:
              data1->value.bValue = dataC.value.bValue;
              break;
            case T_NIL:
              data1->value.bValue = dataC.value.bValue;
              break;
            default:
              if(dataC.varType == T_STRING)
                strFree(&dataC.value.sValue);
              errorFree();
              return EINT;
          }
        }
        else
        {
          if(dataC.varType == T_STRING)
            strFree(&dataC.value.sValue);
          errorFree();
          return EINT;
        }
        if(dataC.varType == T_STRING)
          strFree(&dataC.value.sValue);
        break;

      // ZAPAMATOVANA POLOZKA Z POSLEDNIHO VOLANI
      case I_ASGNR:
        if(retData.hasValue != 0)
        {
          if(data1->varType == T_STRING)
            strFree(&data1->value.sValue);
          data1->hasValue = 1;
          data1->varType = retData.varType;
          switch (retData.varType)
          {
            case T_NUMBER:
              data1->value.dValue = retData.value.dValue;
              break;
            case T_STRING:
              strInitText(&data1->value.sValue, retData.value.sValue.str);
              break;
            case T_BOOLEAN:
              data1->value.bValue = retData.value.bValue;
              break;
            case T_NIL:
              data1->value.bValue = retData.value.bValue;
              break;
            default:
              errorFree();
              return EINT;
          }
          if(retData.varType == T_STRING)
            strFree(&retData.value.sValue);
          retData.varType = T_NOTDEF;
        }
        else
        {
          strFree(&retData.varKey);
          if(retData.varType == T_STRING)
            strFree(&retData.value.sValue);
          retData.varType = T_NOTDEF;
          errorFree();
          return EINT;
        }
        break;

      // READ FUNKCE
      // podla fora sa ma pri cisle chovat inak
      case I_READ:
        if(data1->varType == T_STRING)
          strFree(&data1->value.sValue);
        if(data3->varType == T_NUMBER)
        {
          if(data3->value.dValue < 0)
          {
            errorFree();
            return ESEM;
          }
          strInit(&data1->value.sValue);
          readStrToNUMB(&data1->value.sValue,data3->value.dValue);
          data1->varType = T_STRING;
        }
        else if(data3->varType == T_STRING)
        {
          if(data3->value.sValue.length >= 2)
          {
            if((data3->value.sValue.str[0] == '*') && (data3->value.sValue.str[1] == 'n'))
            {
              if(scanf("%lf",&data1->value.dValue) != 1)
              {
                data1->varType = T_NIL;
                data1->value.bValue = 0;
              }
              else
                data1->varType = T_NUMBER;
            }
            else if((data3->value.sValue.str[0] == '*') && (data3->value.sValue.str[1] == 'l'))
            {
              strInit(&data1->value.sValue);
              readStrToNl(&data1->value.sValue);
              data1->varType = T_STRING;
            }
            else if((data3->value.sValue.str[0] == '*') && (data3->value.sValue.str[1] == 'a'))
            {
              strInit(&data1->value.sValue);
              readStrToEOF(&data1->value.sValue);
              data1->varType = T_STRING;
            }
            else
            {
              errorFree();
              return ESEM;
            }
          }
          else
          {
            errorFree();
            return ESEM;
          }
        }
        else
        {
          errorFree();
          return ESYN;
        }
        data1->hasValue = HAVE_VALUE;
        break;

      // WRITE FUNKCE
      case I_WRITE:
        if (data1->hasValue != 0)
        {
          if(data1->varType == T_NUMBER)
            printf("%g", data1->value.dValue);
          else if (data1->varType == T_STRING)
          {
            // KONECNY AUTOMAT PRO VYPIS
            int writeState = 0;
            for(int i = 0; i < data1->value.sValue.length; i++)
            {
              switch(writeState)
              {
                case 0:
                  if(data1->value.sValue.str[i] == LOMITKO)
                    writeState = 1;
                  else
                    putchar(data1->value.sValue.str[i]);
                  break;
                case 1:
                  if(data1->value.sValue.str[i] == 'n')
                    printf("\n");
                  else if(data1->value.sValue.str[i] == 't')
                    printf("\t");
                  else if(data1->value.sValue.str[i] == LOMITKO)
                    printf("\\");
                  else if (data1->value.sValue.str[i] == UVOZOVKY)
                    printf("\"");
                  else if(data1->value.sValue.str[i] == NOVYRADEK)
                    break;
                  else
                    putchar(data1->value.sValue.str[i]);
                  writeState = 0;
                  break;
              }
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // FUNKCE FTYPE VRACI TYP
      case I_FTYPE:
        if (data1->varType == T_STRING)
          strFree(&data1->value.sValue);
        data1->hasValue = 1;
        switch(data3->varType)
        {
          case T_NOTDEF:
            strInitText(&data1->value.sValue, "nil");
            break;
          case T_NUMBER:
            strInitText(&data1->value.sValue, "number");
            break;
          case T_STRING:
            strInitText(&data1->value.sValue, "string");
            break;
          case T_BOOLEAN:
            strInitText(&data1->value.sValue, "boolean");
            break;
          case T_NIL:
            strInitText(&data1->value.sValue, "nil");
            break;
          default:
            errorFree();
            return EINT;
        }
        data1->varType = T_STRING;
        break;

      // FUNKCE SUBSTRING 
      case I_FSUBS:
        if(data1 != NULL && data2 != NULL && data3 != NULL)
        {
          if(data1->varType == T_STRING && data2->varType == T_NUMBER && data3->varType == T_NUMBER)
          {
            retData.varType = T_STRING;
            retData.hasValue = HAVE_VALUE;
            strInit(&retData.value.sValue);
            strSub(&data1->value.sValue,&retData.value.sValue,(int)data2->value.dValue,(int)data3->value.dValue);
          }
          else
          {
            data1->varType = T_NIL;
            data1->hasValue = 1;
            data1->value.bValue = 0;
          }
        }
        else
        {
          errorFree();
          return EINT;
        }
        break;

      // FUNKCE FIND
      case I_FFIND:
        if(data2->varType == T_STRING && data3->varType == T_STRING)
        {
          if(data2->hasValue != 0 && data2->hasValue != 0)
          {
            int number = 0;
            if(data3->value.sValue.length == 0)
            {
              if(data1->varType == T_STRING)
                strFree(&data1->value.sValue);
              data1->value.dValue = number;
              data1->varType = T_NUMBER;
              data1->hasValue = HAVE_VALUE;
              break;
            }
            else
              number = boyerMooreSearch(&data2->value.sValue, &data3->value.sValue);
            if(data1->varType == T_STRING)
              strFree(&data1->value.sValue);
            if (number == 0)
            {
              data1->value.bValue = 0;
              data1->varType = T_BOOLEAN;
              data1->hasValue = HAVE_VALUE;
            }
            else
            {
              data1->value.dValue = number;
              data1->varType = T_NUMBER;
              data1->hasValue = HAVE_VALUE;
            }
          }
          else
          {
            errorFree();
            return EINT;
          }
        }
        else
        {
          if(data1->varType == T_STRING)
            strFree(&data1->value.sValue);
          data1->varType = T_NIL;
          data1->hasValue = 1;
          data1->value.bValue = 0;
        }
        break;

      // FUNKCE SORT
      case I_FSORT:
        makeCopy(data1,&dataC);
        if(data3->varType == T_STRING)
        {
          dataC.varType = T_STRING;
          sort(&data3->value.sValue,&dataC.value.sValue);
          if(data1->varType == T_STRING)
            strFree(&data1->value.sValue);
          data1->varType = T_STRING;
          data1->hasValue = HAVE_VALUE;
          strInitText(&data1->value.sValue,dataC.value.sValue.str);
        }
        else
        {
          data1->varType = T_NIL;
          data1->hasValue = 1;
          data1->value.bValue = 0;
        }
        if(dataC.varType == T_STRING)
          strFree(&dataC.value.sValue);
        break;

      // PUSH
      case I_PUSH:
        if(data2 != NULL)
        {
          // ked pushujeme premenne pre funkciu potrebujeme data1 data2
          // pre originalny nazov parametru
          pushData.varType = data2->varType;
          strInitText(&pushData.varKey,data1->varKey.str);
          if(data2->varType == T_STRING)
            strInitText(&pushData.value.sValue,data2->value.sValue.str);
          else if(data2->varType == T_NIL)
            pushData.value.bValue = 0;
          else if(data2->varType == T_BOOLEAN)
            pushData.value.bValue = data2->value.bValue;
          else if(data2->varType == T_NUMBER)
            pushData.value.dValue = data2->value.dValue;
          pushData.hasValue = HAVE_VALUE;
          hListInsert(&list2, &pushData, S_VAR);
          strFree(&pushData.varKey);
          if(pushData.varType == T_STRING)
            strFree(&pushData.value.sValue);
        }
        // push lokalnej premennej
        else if(data1 != NULL)
          hListInsert(&list2, data1, S_VAR);
        break;

      // RETURN
      case I_RETURN:
        retData.varType = data1->varType;
        retData.hasValue = data1->hasValue;
        switch(retData.varType)
        {
          case T_STRING:
            strInitText(&retData.value.sValue, data1->value.sValue.str);
            break;
          case T_NUMBER:
            retData.value.dValue = data1->value.dValue;
            break;
          case T_BOOLEAN:
            retData.value.bValue = data1->value.bValue;
            break;
          case T_NIL:
            retData.value.bValue = 0;
            break;
        }
        hListClearToLab(&list2);
        listGoto(iList,jStackPop(&jStack));
        break;

      // KONEC PROGRAMU
      case I_END:
        errorFree();
        return 0;
        break;
    }

    // ukazatele na NULL
    data1 = NULL;
    data2 = NULL;
    data3 = NULL;
    var = NULL;

    // nacteme dalsi instrukci
    listNext(iList);
  }
  return EIOK;
}

/**
 * Vytvori kopiu dat. Pro prikazy ako a = a alebo a = a .. b
 */
void makeCopy(tData *data, tData *copy)
{
  copy->hasValue = data->hasValue;
  copy->varType = data->varType;
  if(copy->varType == T_NUMBER)
    copy->value.dValue = data->value.dValue;
  else if(copy->varType == T_STRING)
    strInitText(&copy->value.sValue,data->value.sValue.str);
  else if(copy->varType == T_BOOLEAN)
    copy->value.bValue = data->value.bValue;
  else if(copy->varType == T_NIL)
    copy->value.bValue = 0;
  else if(copy->varType == T_NOTDEF)
    copy->value.bValue = 0;
}

/**
 * Funkce pri chybe uvolni vsechny dynamicky vytvorene promenne v interpret.c
 */
void errorFree()
{
  hListDispose(g_list);
  jStackDispose(g_jStack);
  if(g_retData->varType == T_STRING)
    strFree(&g_retData->varKey);
}
/*** Konec souboru interpret.c ***/
