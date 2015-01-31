/**
 * Projekt: IFJ11 kompilator.
 * @file: parser.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro parser.
 */


#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "ial.h"
#include "scaner.h"
#include "ilist.h"
#include "expr.h"
#include "interpret.h"
#include "parser.h"
#include "main.h"
#include <string.h>

int token; // ukladani tokenu
int asgnTyp = RVNull; // udava, zda expr nacetl write nebo read
string attr; // atributy tokenu
BNode *btree;
string nazevFunkce; // prefix pro promenne
tData *data;
int uniqueKey = 1; // slouzi ke generovani unikatnich id
int uniqueKeyNumber = 1;
int isReturn = 0;

tListOfInstr *list;
tFuncList *fList;

void vypis() {
  printf("%i - %s\n", token, attr.str);
}

void vypisData(tData *data) {
  if(data != NULL) {
    printf("--------------\n");
    printf("nazev: %s\n", data->varKey.str);
    printf("typ: %i\n", data->varType);
    printf("ma hodnotu: %i\n", data->hasValue);
    printf("pocet parametru: %i\n", data->prmCount);
    if(data->varType == T_NUMBER) {
      printf("hodnota: %f\n", data->value.dValue);
    } else if(data->varType == T_STRING) {
      printf("hodnota: %s\n", data->value.sValue.str);
    } else if(data->varType == T_BOOLEAN) {
      printf("hodnota: %i\n", data->value.bValue);
    }
  } else {
    printf("Nebyl nalezen odpovidajici identifikator.\n");
  }
}

// E == epsilon
// 1. <program> -> function <o_function><m_function>$
int program(tListOfInstr *list2, BNode *node) {
  strInit(&attr);
  int result = EIOK;
  tFuncList fList2;

  // sdileni stromu s expr.c
  nastavStrom(node);

  // priradeni do globalnich promennych
  fList = &fList2;
  list = list2;
  btree = node;

  initFList(fList); // seznam se vsemi funkcemi

  if((token = getNextToken(&attr)) == ELEX) { strFree(&attr); return ELEX; }
  // jako prvni musi prijit "function"
  if(token == FUNCTION) {
    if((result = oFunction()) != EIOK) {
      disposeFList(fList);
      strFree(&attr);
      return result;
    }
    disposeFList(fList);
  } else {
    strFree(&attr);
    return ESYN;
  }

  generateInstruction(I_END, NULL, NULL, NULL);

  // zpracovali jsme cely program, ocekavame konec souboru
  if((token = getNextToken(&attr)) != END_OF_FILE) result = ESYN;

  strFree(&attr);

  return result;
}

// 2. o_function> -> f_id(<params>)<body>end function <o_function>
// 3. <o_function> -> E
int oFunction() {
  int result = EIOK;
  int paramPocet = 0;
  tData novaPolozka; // novy zaznam do tabulky symbolu
  string funcName;
  string pomString; // pouzije se pri vytvareni return nil
  tFuncListItem *pomFElem;

  isReturn = 0;

  if((token = getNextToken(&attr)) == ELEX) return ELEX;

  if(token == ID) {
    // musime odlisit pripad, kdy se jedna o funkci main
    if(strCmpConstStr(&attr, "main") == 0) {
      result = mFunction();
    } else {

      // ulozime si nazev funkce a pouzijeme ho jako prefix do TS
      strInitText(&nazevFunkce, attr.str);
      strInitText(&funcName, attr.str);
      // kazdou funkci skladujeme v seznamu funkci, slouzi pro kontrolu zda byly definovany
      pomFElem = updateFuncList(fList, funcName, 1);
      if(pomFElem != NULL) {
        strFree(&funcName);
        strFree(&nazevFunkce);
        return ESEM;
      } else {
        pomFElem = insertFLast(fList, funcName, 1);
      }
      strFree(&funcName);

      if((token = getNextToken(&attr)) == ELEX) {
        strFree(&nazevFunkce);
        return ELEX;
      }
      if(token != LEVA_NORMAL_ZAV) {
        strFree(&nazevFunkce);
        return ESYN;
      }

      // ulozime informace o funkci do struktury
      novaPolozka.hasValue = HAVE_VALUE;
      novaPolozka.varType = T_FUNC;
      strInitText(&novaPolozka.value.sValue, ";");
      // neni uz tato funkce definovana?
      if(searchBtreeStr(btree, &nazevFunkce) != NULL) {
        strFree(&nazevFunkce);
        return ESYN;
      }
      // pridame funkci do TS
      data = getInsertBtree(btree, &nazevFunkce, &novaPolozka);
      strFree(&novaPolozka.value.sValue);
      strClear(&data->value.sValue);

      // zkontrolujeme parametry
      if((result = params(&paramPocet)) != EIOK) {
        strFree(&nazevFunkce);
        return result;
      }

      if(token != PRAVA_NORMAL_ZAV) {
        strFree(&nazevFunkce);
        return ESYN;
      }

      data->prmCount = paramPocet;

      generateInstruction(I_LAB, NULL, NULL, NULL);
      pomFElem->adresaFunkce = listGetPointerLast(list); // ulozime adresu zacatku funkce
      generateInstruction(I_LAB_S, NULL, NULL, NULL);

      // zpracujeme telo funkce
      if((result = body()) != EIOK) {
        strFree(&nazevFunkce);
        return result;
      }
      strFree(&nazevFunkce);

      // pokud ve funkci nebyl return, musi vracet nil "rucne"
      if(!isReturn) {
        novaPolozka.varType = T_NIL;
        pomString = genUniqueKeyNumber();
        data = getInsertBtree(btree, &pomString, &novaPolozka);
        strFree(&pomString);
        generateInstruction(I_RETURN, data, NULL, NULL);
      }


      // funkce musi koncit slovem end
      if(token != END) return ESYN;

      // spustime pravidlo znova, bud prijde dalsi funkce nebo main, musi ale prijit function
      if((token = getNextToken(&attr)) == ELEX) return ELEX;
      if(token != FUNCTION) return ESYN;
      result = oFunction();

    }
  } else {
    result = ESYN;
  }

  return result;
}

// 4. <m_function> -> main(<params>)<body>end;
int mFunction() {
  int result = EIOK;
  int paramPocet = 0;
  tData novaPolozka; // novy zaznam do tabulky symbolu

  // ulozime si nazev funkce a pouzijeme ho jako prefix do TS
  strInitText(&nazevFunkce, attr.str);

  // main uz jsme nacetli ve funkci oFunction, ocekavame (
  if((token = getNextToken(&attr)) == ELEX) { strFree(&nazevFunkce); return ELEX; }
  if(token != LEVA_NORMAL_ZAV) { strFree(&nazevFunkce); return ESYN; }

  // ulozime informace o funkci do struktury
  novaPolozka.hasValue = HAVE_VALUE;
  novaPolozka.varType = T_FUNC;
  strInitText(&novaPolozka.value.sValue, ";");
  // neni uz tato funkce definovana?
  if(searchBtreeStr(btree, &nazevFunkce) != NULL) { strFree(&nazevFunkce); return ESYN; }
  // pridame funkci do TS
  data = getInsertBtree(btree, &nazevFunkce, &novaPolozka);
  strFree(&novaPolozka.value.sValue);
  strClear(&data->value.sValue);


  // zkontrolujeme parametry
  if((result = params(&paramPocet)) != EIOK) { strFree(&nazevFunkce); return result; }

  if(token != PRAVA_NORMAL_ZAV) { strFree(&nazevFunkce); return ESYN; }

  data->prmCount = paramPocet; // pocet parametru uz lze doplnit

  generateInstruction(I_LAB_MAIN, NULL, NULL, NULL);

  // zpracujeme telo funkce
  if((result = body()) != EIOK) { strFree(&nazevFunkce); return result; }

  // funkce main musi koncit slovem end; (vcetne stredniku!)
  if(token != END) { strFree(&nazevFunkce); return ESYN; }
  if((token = getNextToken(&attr)) == ELEX) { strFree(&nazevFunkce); return ELEX; }
  if(token != STREDNIK) result = ESYN;

  // musime uvolnovat pamet
  strFree(&nazevFunkce);

  return result;
}

// 5. <params> -> param <params_n>
// 6. <params> -> E
int params(int *paramPocet) {
  int result = EIOK;
  tData *funkce;
  string nazevProm;
  tData novaPolozka; // novy zaznam do tabulky symbolu

  // musi prijit id, pokud neprijde, jeste dame programu jednu sanci v pravidle pro funkce
  // tzn bude muset prijit ')'
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  switch(token) {
    case ID:
      strInit(&nazevProm);
      funkce = searchBtreeStr(btree, &nazevFunkce);
      strAddText(&funkce->value.sValue, attr.str);
      strAddChar(&funkce->value.sValue, ';');
      *paramPocet += 1; // zvysime pocet parametru funkce

      // parametry je treba pridat do TS
      strClear(&nazevProm);
      strAddText(&nazevProm, nazevFunkce.str);
      strAddChar(&nazevProm, '_');
      strAddText(&nazevProm, attr.str);

      novaPolozka.hasValue = HAVENT_VALUE;
      novaPolozka.varType = T_NIL;

      // pridame promennou do TS
      insertDataBtree(btree, &nazevProm, &novaPolozka);
      strFree(&nazevProm);

      // zpracujeme dalsi parametry
      result = paramsN(paramPocet);
      break;
    default:
      funkce = searchBtreeStr(btree, &nazevFunkce);
      strAddChar(&funkce->value.sValue, ';');
  }

  return result;
}

// 7. <params_n> -> ,param <params_n>
// 8. <params_n> -> E
int paramsN(int *paramPocet) {
  int result = EIOK;
  tData *funkce;
  string nazevProm;
  tData novaPolozka; // novy zaznam do tabulky symbolu

  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  // musi prijit carka a identifikator, ostatni zpracujeme v pravidle pro funkci
  if(token == CARKA) {
    if((token = getNextToken(&attr)) == ELEX) return ELEX;
    if(token == ID) {
      strInit(&nazevProm);
      funkce = searchBtreeStr(btree, &nazevFunkce);
      strAddText(&funkce->value.sValue, attr.str);
      strAddChar(&funkce->value.sValue, ';');
      *paramPocet += 1;

      // parametry je treba pridat do TS
      strClear(&nazevProm);
      strAddText(&nazevProm, nazevFunkce.str);
      strAddChar(&nazevProm, '_');
      strAddText(&nazevProm, attr.str);

      novaPolozka.hasValue = HAVENT_VALUE;
      novaPolozka.varType = T_NIL;

      // pridame promennou do TS
      insertDataBtree(btree, &nazevProm, &novaPolozka);
      strFree(&nazevProm);

      // zpracujeme dalsi parametry
      result = paramsN(paramPocet);
    } else {
      result = ESYN;
    }
  }

  return result;
}

// 9. <body> -> <var_declarations><stat_list>
int body() {
  int result = EIOK;

  if((result = varDecl()) != EIOK) return result;
  generateInstruction(I_LAB_F, NULL, NULL, NULL);
  if(token != END) { result = statList(); }

  return result;
}

// 10. <var_declarations> -> local id <decl> <var_declarations>
// 11. <var_declarations> -> E
int varDecl() {
  int result = EIOK;
  string nazevProm;
  tData novaPolozka; // novy zaznam do tabulky symbolu
  tElement *pomData = NULL;

  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if(token == LOCAL) {
    if((token = getNextToken(&attr)) == ELEX) return ELEX;
    // po local musi prijit id
    if(token == ID) {
      // ulozime nazev funkce
      strInitText(&nazevProm, nazevFunkce.str);
      strAddChar(&nazevProm, '_');
      strAddText(&nazevProm, attr.str);

      novaPolozka.hasValue = HAVENT_VALUE;
      novaPolozka.varType = T_NIL;

      data = searchBtreeStr(btree, &nazevProm);
      if(data != NULL) {
        strFree(&nazevProm);
        return ESYN;
      }

      // je pripustne promennou rovnou inicializovat, jinak bude decl strednik
      if((result = decl(&pomData)) != EIOK) return result;
      if(pomData != NULL) novaPolozka.varType = pomData->data->varType;

      // pridame promennou do TS
      data = getInsertBtree(btree, &nazevProm, &novaPolozka);
      strFree(&nazevProm);

      generateInstruction(I_PUSH, data, NULL, NULL);

      if(pomData != NULL) generateInstruction(I_ASGN, data, NULL, pomData->data);

      // mohou nasledovat dalsi deklarace
      result = varDecl();
    } else {
      return ESYN;
    }
  }

  return result;
}

// 12. <decl> -> ;
// 13. <decl> -> = <expr>;
int decl(tElement **pomData) { // dodelat true/false
  int result = EIOK, asgnTyp;

  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  switch(token) {
    case STREDNIK:
      break;
    case ROVNASE:
      asgnTyp=RVNull;
      *pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);

      if((*pomData)->error != EIOK) {
        result = (*pomData)->error;
        free((*pomData));
        return result;
      }

      break;
    // pokud neni strednik ani =, je to chyba
    default:
      result = ESYN;
      break;
  }

  return result;
}

// 14. <stat_list> -> <state><stat_list>
// 15. <stat_list> -> E
int statList() {
  int result = EIOK;

  // musime zjistit, jestli neni cast s prikazy prazdna
  if(token != END && token != ELSE && token != UNTIL) {
    if((result = stat()) != EIOK) return result;
    // mohou nasledovat dalsi prikazy
    result = statList();
  }

  return result;
}

// 16. <state> -> write(<expr><exprs>);
// 17. <state> -> while <expr> do <stat_list> end;
// 18. <state> -> repeat <stat_list> until <expr>;
// 19. <state> -> if <expr> then <stat_list> else <stat_list> end;
// 20. <state> -> return <expr>
// 21. <state> -> E
int stat() {
  int result = EIOK;
  tElement *pomData;

  switch(token) {
    // promenna = xxx
    case ID:
      if((result = assign()) != EIOK) return result;
      break;
    case WRITE:
      if((result = write()) != EIOK) return result;
      break;
    case WHILE:
      if((result = gWhile()) != EIOK) return result;
      break;
    case REPEAT:
      if((result = gRepeat()) != EIOK) return result;
      break;
    case IF:
      if((result = gIf()) != EIOK) return result;
      break;
    case RETURN:
      pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list); // zpracovani vyrazu return <expr>
      if(pomData->error != EIOK) return pomData->error;
      if(strcmp(nazevFunkce.str, "main") != 0) {
        isReturn = 1;
        generateInstruction(I_RETURN, pomData->data, NULL, NULL);
      } else {
        generateInstruction(I_END, NULL, NULL, NULL);
      }
      free(pomData);
      break;
    case END:
    case ELSE:
    case UNTIL:
      break;
    default:
      result = ESYN;
      break;
  }

  if((token = getNextToken(&attr)) == ELEX) return ELEX;

  return result;
}

// 22. <state> -> id = <assign>;
// 23. <assign> -> <expr>
// 24. <assign> -> f_id(<params>)
// 25. <assign> -> read(<read>)
// 26. <read> -> number
// 27. <read> -> string
int assign() {
  int result = EIOK;
  tElement *pomData; // pomocna polozka pro hledani ve stromu
  tData *readData;
  tData *dataProm;
  tData novaPolozka; // nova polozka - pomocny vypocet
  string pomString; // pomocny retezec pro vytvoreni nazvu promenne
  tFuncListItem *pomFElem;
  void *navesti;
  int i, j;

  /* --------------------------------------------------------------- */
  // tElement *data (globalni) - obsahuje udaje o funkci ze stromu, nepouzivat
  // tElement *pomData - vysledek <expr>, nepouzivat
  // tFuncListItem *pomFElem - pouziva se pro praci se seznamem funkci, nepouzivat
  // tData *dataProm - data o promenne, do ktere se uklada, nepouzivat
  // ------------------------------------------------------------------
  // tData *readData - pomocna promenna pro nacitani ze stromu, lze pouzit
  // tData novaPolozka - slouzi pro vkladani do stromu, lze pouzit
  // string pomString - pomocny retezec pro hledani, lze pouzit
  /* --------------------------------------------------------------- */

  // musime overit, zda tato promenna do ktere se prirazuje, existuje
  strInitText(&pomString, nazevFunkce.str);
  strAddChar(&pomString, '_');
  strAddText(&pomString, attr.str);
  if((dataProm = searchBtreeStr(btree, &pomString)) == NULL) {
    strFree(&pomString);
    return ESYN;
  }

  if((token = getNextToken(&attr)) == ELEX) {
    strFree(&pomString);
    return ELEX;
  }

  // musi prijit = (id uz bylo nacteno)
  if(token == ROVNASE) {

    // expr vrati rizeni v pripade, ze nacte read, to zpracujeme nize
    pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);

    if(pomData->error != EIOK) {
      result = pomData->error;
      free(pomData);
      strFree(&pomString);
      return result;
    }

    // pokud se jedna o prikaz read
    if(asgnTyp == RVRead) {
      // 24. <assign> -> read(<read>)
      if((token = getNextToken(&attr)) == ELEX) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ELEX;
      }
      if(token != LEVA_NORMAL_ZAV) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ESYN;
      }
      if((token = getNextToken(&attr)) == ELEX) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ELEX;
      }

      strFree(&pomString);
      pomString = genUniqueKey(nazevFunkce); // vytvorime pomocnou promennou pro read
      novaPolozka.prmCount = 0;
      novaPolozka.varType = 0;
      novaPolozka.hasValue = 0;
      insertDataBtree(btree, &pomString, &novaPolozka); // sup s tim do TS
      readData = searchBtreeStr(btree, &pomString);

      // zpracovani parametru read()
      switch(token) {
        // bud muze byt jako parametr cislo (pocet nactenych znaku)
        case CISLO:
          readData->varType = T_NUMBER;
          readData->value.dValue = atof(attr.str);
          break;
        //nebo muze parametr udavat format nacitanych dat
        case TEXT_RETEZEC:
          readData->varType = T_STRING;
          strInitText(&readData->value.sValue, attr.str);
          break;
        default:
          // pokud neprijde cislo ani textovy retezec, je to chyba
          free(pomData);
          strFree(&pomString);
          return ESYN;
          break;
      }

      // ted musi prijit ");"
      if((token = getNextToken(&attr)) == ELEX) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ELEX;
      }
      if(token != PRAVA_NORMAL_ZAV) {
        free(pomData);
        strFree(&pomString);
        return ESYN;
      }
      if((token = getNextToken(&attr)) == ELEX) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ELEX;
      }
      if(token != STREDNIK) {
        free(pomData);
        strFree(&pomString);
        return ESYN;
      }

      generateInstruction(I_READ, dataProm, NULL, readData);

    // pokud je to prirazeni funkce
    } else if(asgnTyp == RVFunc) {
      data = searchBtreeStr(btree, &pomData->data->varKey);

      // vestavena funkce type
      if(strCmpConstStr(&pomData->data->varKey, "type") == 0) {
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

        // pokud to nebude promenna, pripravime si pomocny zaznam v TS
        if(token == TEXT_RETEZEC || token == CISLO || token == TRUE || token == FALSE) {
            strClear(&pomString);
            pomString = genUniqueKeyNumber();
            novaPolozka.hasValue = HAVE_VALUE;
            readData = getInsertBtree(btree, &pomString, &novaPolozka);
        }

        switch(token) {
          case ID:
            strClear(&pomString);
            strAddText(&pomString, nazevFunkce.str);
            strAddChar(&pomString, '_');
            strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
            if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
              if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
              strFree(&pomData->data->varKey);
              free(pomData);
              strFree(&pomString);
              return ESEM;
            }
            break;
          // do pomocneho zaznamu v TS doplnime datovy typ
          case TEXT_RETEZEC:
            readData->varType = T_STRING;
            break;
          case CISLO:
            readData->varType = T_NUMBER;
            break;
          case TRUE:
          case FALSE:
            readData->varType = T_BOOLEAN;
            break;
          default:
            free(pomData);
            strFree(&pomString);
            return ESYN;
        } // switch(token)

        generateInstruction(I_FTYPE, dataProm, NULL, readData);

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

      // vestavena funkce substr
      } else if (strCmpConstStr(&pomData->data->varKey, "substr") == 0) {
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

        // zkontrolujeme prvni parametr, muze byt ID nebo retezec
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == TEXT_RETEZEC) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_STRING;
          strInitText(&readData->value.sValue, attr.str);
        } else {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        generateInstruction(I_FSUBS, readData, NULL, NULL);
        navesti = listGetPointerLast(list); // do instrukce jeste pozdeji ulozime dalsi adresy

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
        if(token != CARKA) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESYN;
        }

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
        // zkontrolujeme druhy parametr, muze byt ID nebo cislo
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == CISLO) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_NUMBER;
          readData->value.dValue = atof(attr.str);
        } else {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        ((tInstr*) navesti)->addr2 = readData;

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
        if(token != CARKA) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESYN;
        }

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
        // zkontrolujeme druhy parametr, muze byt ID nebo cislo
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == CISLO) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_NUMBER;
          readData->value.dValue = atof(attr.str);
        } else {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        ((tInstr*) navesti)->addr3 = readData;

        generateInstruction(I_ASGNR, dataProm, NULL, NULL);

        // jeste nacteme )
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

      // vestavena funkce find
      } else if(strCmpConstStr(&pomData->data->varKey, "find") == 0) {
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

        // zkontrolujeme prvni parametr, muze byt ID nebo retezec
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == TEXT_RETEZEC) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_STRING;
          strInitText(&readData->value.sValue, attr.str);
        } else {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        // vytvorime instrukci, jeste do ni pozdeji zapiseme treti adresu
        generateInstruction(I_FFIND, dataProm, readData, NULL);
        navesti = listGetPointerLast(list);

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
        if(token != CARKA) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESYN;
        }

        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

        // zkontrolujeme druhy parametr, muze byt ID nebo retezec
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == TEXT_RETEZEC) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_STRING;
          strInitText(&readData->value.sValue, attr.str);
        } else {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        ((tInstr*) navesti)->addr3 = readData;
        // jeste nacteme )
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

      } else if(strCmpConstStr(&pomData->data->varKey, "sort") == 0) {
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }

        // zkontrolujeme parametr, muze byt ID nebo retezec
        if(token == ID) {
          strClear(&pomString);
          strAddText(&pomString, nazevFunkce.str);
          strAddChar(&pomString, '_');
          strAddText(&pomString, attr.str); // sestavime nazev promenne vuci dane funkci
          if((readData = searchBtreeStr(btree, &pomString)) == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }
        } else if(token == TEXT_RETEZEC) {
          strClear(&pomString);
          pomString = genUniqueKeyNumber(); // retezec se ulozi do pomocne promenne v TS
          novaPolozka.hasValue = HAVE_VALUE;
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          readData->varType = T_STRING;
          strInitText(&readData->value.sValue, attr.str);
        } else {
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        generateInstruction(I_FSORT, dataProm, NULL, readData);

        // jeste nacteme )
        if((token = getNextToken(&attr)) == ELEX) {
          if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
          strFree(&pomData->data->varKey);
          free(pomData);
          strFree(&pomString);
          return ELEX;
        }
      } else {
        pomFElem = updateFuncList(fList, pomData->data->varKey, 0); // ulozime ji do seznamu funkci
        if(pomFElem == NULL) {
          free(pomData);
          strFree(&pomString);
          return ESEM;
        }

        // cyklus pro nacitani parametru (podle prmCount)
        for(i = 0; i < data->prmCount; i++) {
          strClear(&pomString);
          if(token != PRAVA_NORMAL_ZAV) { // pokud neni ), tak lze nacist dalsi dalsi parametry
            if((token = getNextToken(&attr)) == ELEX) {
              if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
              strFree(&pomData->data->varKey);
              free(pomData);
              strFree(&pomString);
              return ELEX;
            }
            // parametr muze byt jeden z techto typu
            if(token != ID && token != TEXT_RETEZEC && token != CISLO && token != TRUE && token != FALSE && token != NIL) {
              if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
              strFree(&pomData->data->varKey);
              free(pomData);
              strFree(&pomString);
              return ESYN;
            }
          } else { // kdyz je ), musi se doplnit nil
            for(j = i; j < data->prmCount; j++) { // prochazi zbytek promennych, ktere chybi
              strClear(&pomString);
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_NIL;
              novaPolozka.hasValue = HAVE_VALUE;
              readData = getInsertBtree(btree, &pomString, &novaPolozka); // nil promenne
            }
            break;
          }

          // je potreba najit ve stromu dany parametr, pokud to neni ID, tak pridat
          switch(token) {
            case ID:
              strAddText(&pomString, nazevFunkce.str);
              strAddChar(&pomString, '_');
              strAddText(&pomString, attr.str);
              readData = searchBtreeStr(btree, &pomString);
              break;
            case CISLO:
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_NUMBER;
              novaPolozka.hasValue = HAVE_VALUE;
              novaPolozka.value.dValue = atof(attr.str);
              readData = getInsertBtree(btree, &pomString, &novaPolozka);
              break;
            case TEXT_RETEZEC:
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_STRING;
              novaPolozka.hasValue = HAVE_VALUE;
              strInitText(&novaPolozka.value.sValue, ";");
              readData = getInsertBtree(btree, &pomString, &novaPolozka);
              strFree(&novaPolozka.value.sValue);
              strClear(&readData->value.sValue);
              strAddText(&readData->value.sValue, attr.str);
              break;
            case TRUE:
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_BOOLEAN;
              novaPolozka.hasValue = HAVE_VALUE;
              novaPolozka.value.bValue = 1;
              readData = getInsertBtree(btree, &pomString, &novaPolozka);
              break;
            case FALSE:
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_BOOLEAN;
              novaPolozka.hasValue = HAVE_VALUE;
              novaPolozka.value.bValue = 0;
              readData = getInsertBtree(btree, &pomString, &novaPolozka);
              break;
            case NIL:
              pomString = genUniqueKeyNumber();
              novaPolozka.varType = T_NIL;
              novaPolozka.hasValue = HAVE_VALUE;
              novaPolozka.value.bValue = 0;
              readData = getInsertBtree(btree, &pomString, &novaPolozka);
              break;
          }

          // neexistuje ID nebo chyba pri pridani pomocne promenne
          if(readData == NULL) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ESEM;
          }

          // zkopirujeme jej do stromu s nazvem, ktery se pouziva uvnitr funkce
          novaPolozka.hasValue = readData->hasValue;
          novaPolozka.varType = readData->varType;
          novaPolozka.value.dValue = readData->value.dValue;

          // jeste si musime ziskat nazev parametru pouzivany uvnitr funkce
          strClear(&pomString);
          strAddText(&pomString, data->varKey.str);
          strAddChar(&pomString, '_');
          strSearchParam(&data->value.sValue, &pomString, i);

          // nejprve vlozime puvodni nazev promenne
          generateInstruction(I_PUSH, NULL, readData, NULL);
          navesti = listGetPointerLast(list);
          readData = getInsertBtree(btree, &pomString, &novaPolozka);
          // a pak vlozime take nazev promenne pouzity uvnitr funkce
          ((tInstr*) navesti)->addr1 = readData;

          if((token = getNextToken(&attr)) == ELEX) {
            if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
            strFree(&pomData->data->varKey);
            free(pomData);
            strFree(&pomString);
            return ELEX;
          }
        }

        if(i > 0) {
        while(token != PRAVA_NORMAL_ZAV) {
          if((token = getNextToken(&attr)) == ELEX) {
            free(pomData);
            strFree(&pomString);
            return ELEX;
          }
          if(token != ID && token != TEXT_RETEZEC && token != CISLO && token != TRUE && token != FALSE && token != NIL) {
            free(pomData);
            strFree(&pomString);
            return ESYN;
          }
          if((token = getNextToken(&attr)) == ELEX) {
            free(pomData);
            strFree(&pomString);
            return ELEX;
          }
          if(token != CARKA && token != PRAVA_NORMAL_ZAV) {
            free(pomData);
            strFree(&pomString);
            return ESYN;
          }
        }
        } else {
          if((token = getNextToken(&attr)) == ELEX) {
            free(pomData);
            strFree(&pomString);
            return ELEX;
          }
          if(token != PRAVA_NORMAL_ZAV) {
            free(pomData);
            strFree(&pomString);
            return ESYN;
          }
        }

        generateInstruction(I_JUMP, pomFElem->adresaFunkce, NULL, NULL);
        // ulozime, kam se ma funkce vratit po skonceni
        navesti = listGetPointerLast(list);
        ((tInstr*) navesti)->addr3 = navesti;

        generateInstruction(I_ASGNR, dataProm, NULL, NULL);
      } // konec casti kdyz se jedna o beznou funkci

      // po parametrech musi prijit ;
      if((token = getNextToken(&attr)) == ELEX) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ELEX;
      }
      if(token != STREDNIK) {
        if(pomData->data->varType == T_STRING) strFree(&pomData->data->value.sValue);
        strFree(&pomData->data->varKey);
        free(pomData);
        strFree(&pomString);
        return ESYN;
      }

      strFree(&pomData->data->varKey);
      free(pomData->data);

    // pokud to neni ani funkce, ani read, ale vyraz
    } else {
      generateInstruction(I_ASGN, dataProm, NULL, pomData->data);
    }

  } else {
    result = ESYN;
  }

  strFree(&pomString);

  return result;
}

// 16. <state> -> write(<expr><exprs>);
int write() {
  int result = EIOK;
  tElement *pomData;

  // ocekavame ( protoze write uz bylo nacteno
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if(token == LEVA_NORMAL_ZAV) {

    while(asgnTyp != RVRPara) { // dokud neprijde ) nacitej vyrazy
      asgnTyp = RVWrite;
      pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);

      if(pomData->error != EIOK) {
        result = pomData->error;
        free(pomData);
        return result;
      }
      free(pomData);
    }

    asgnTyp = RVNull; // expr by jinak znovu ocekaval write

    //musi koncit strednikem
    if((token = getNextToken(&attr)) == ELEX) return ELEX;
    if(token != STREDNIK) result = ESYN;
  } else {
    result = ESYN;
  }

  return result;
}

// 17. <state> -> while <expr> do <stat_list> end;
int gWhile() {
  int result = EIOK;
  tElement *pomData; // pomocna polozka pro hledani ve stromu
  void *labPrvni;
  void *labNaKonec;

  // navesti pro iteraci while
  generateInstruction(I_LAB, NULL, NULL, NULL);
  labPrvni = listGetPointerLast(list);

  asgnTyp = RVWhile;

  // while uz jsme nacetli, ocekavame vyraz
  pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);
  if(pomData->error != EIOK) {
    result = pomData->error;
    free(pomData);
    return result;
  }

  // test, zda se ma provest dalsi iterace while
  generateInstruction(I_JMPN, NULL, NULL, pomData->data);
  labNaKonec = listGetPointerLast(list);

  free(pomData);

  // statList potrebuje nacist pred zavolanim novy token
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if((result = statList()) != EIOK) return result;

  generateInstruction(I_JUMP, labPrvni, NULL, NULL);
  generateInstruction(I_LAB, NULL, NULL, NULL);
  ((tInstr*) labNaKonec)->addr1 = listGetPointerLast(list);

  // while konci end;
  if(token != END) return ESYN;
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if(token != STREDNIK) result = ESYN;

  return result;
}

int gRepeat() {
  int result = EIOK;
  tElement *pomData; // pomocna polozka pro hledani ve stromu
  void *labPrvni;

  // navesti pro iteraci repeat
  generateInstruction(I_LAB, NULL, NULL, NULL);
  labPrvni = listGetPointerLast(list);

  // statList potrebuje nacist pred zavolanim novy token
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if((result = statList()) != EIOK) return result;

  // until uz jsme nacetli, ocekavame vyraz
  pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);
  if(pomData->error != EIOK) {
    result = pomData->error;
    free(pomData);
    return result;
  }

  token = STREDNIK;

  // test, zda se ma provest dalsi iterace while
  generateInstruction(I_JMPN, labPrvni, NULL, pomData->data);
  free(pomData);

  return result;
}

// 18. <state> -> if <expr> then <stat_list> else <stat_list> end;
int gIf() {
  int result = EIOK;
  tElement *pomData;
  void *labElse;
  void *labNaKonec;
  void *labKonec;
  void *labPrvni;

  asgnTyp = RVIf;

  // if uz jsme nacetli, ocekavame vyraz
  pomData = syntAnalExpr(&asgnTyp, nazevFunkce, list);
  if(pomData->error != EIOK) {
    result = pomData->error;
    free(pomData);
    return result;
  }

  // je potreba ulozit adresu instrukce, aby se dala doplnit adresa else
  generateInstruction(I_JMPN, NULL, NULL, pomData->data);
  labPrvni = listGetPointerLast(list);

  free(pomData);

  // statList potrebuje nacist pred zavolanim novy token
  if((token = getNextToken(&attr)) == ELEX) return ELEX;
  if((result = statList()) != EIOK) return result;

  // je potreba ulozit adresu instrukce, aby se dala doplnit adresa end;
  generateInstruction(I_JUMP, NULL, NULL, NULL);
  labNaKonec = listGetPointerLast(list);

  // nyni musi prijit ELSE <prikazy> END;
  if(token == ELSE) {
    // ziskame adresu else a ulozime do skoku pri nesplneni podminky
    generateInstruction(I_LAB, NULL, NULL, NULL);
    labElse = listGetPointerLast(list);
    ((tInstr*) labPrvni)->addr1 = labElse;

    // statList potrebuje nacist pred zavolanim novy token
    if((token = getNextToken(&attr)) == ELEX) return ELEX;
    if((result = statList()) != EIOK) return result;
    // po prikazech musi prijit end;
    if(token != END) return ESYN;

    // ziskame adresu end a ulozime do skoku pri splneni podminky (pred else)
    generateInstruction(I_LAB, NULL, NULL, NULL);
    labKonec = listGetPointerLast(list);
    ((tInstr*) labNaKonec)->addr1 = labKonec;

    if((token = getNextToken(&attr)) == ELEX) return ELEX;
    if(token != STREDNIK) result = ESYN;
  } else {
    return ESYN;
  }

  return result;
}

void initFList(tFuncList *fList) {
  fList->First = NULL;
  fList->Last = NULL;
}

void disposeFList(tFuncList *fList) {
  tFuncListItem *tmp;

  while(fList->First != NULL) {
    tmp = fList->First->nextItem;
    strFree(&fList->First->funcName);
    free(fList->First);
    fList->First = tmp;
  }
}

tFuncListItem *insertFLast(tFuncList *fList, string fName, int def) {
  tFuncListItem *newItem;

  newItem = malloc(sizeof(tFuncListItem));
  strInitText(&newItem->funcName, fName.str);
  newItem->isDef = def;
  newItem->nextItem = NULL;

  if(fList->First == NULL) {
    fList->First = newItem;
  } else {
    fList->Last->nextItem = newItem;
  }
  fList->Last = newItem;

  return newItem;
}

void printFuncList(tFuncList *fList) {
  for(tFuncListItem *First = fList->First; First != NULL; First = First->nextItem)
    printf("funkce: %s - %i\n", First->funcName.str, First->isDef);
}

tFuncListItem *updateFuncList(tFuncList *fList, string fName, int def) {
  int najdi = 0;

  tFuncListItem *First;

  for(First = fList->First; First != NULL; First = First->nextItem) {
    if(strcmp(fName.str, First->funcName.str) == 0) {
      najdi = 1;
      break;
    }
  }

  if(najdi == 1) {
    if(First->isDef != 1 && def != 0)
      First->isDef = def;
  } else {
    return NULL;
  }

  if(First == NULL) First = fList->Last;

  return First;
}
