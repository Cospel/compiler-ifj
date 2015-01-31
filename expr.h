/**
 * Projekt: IFJ11 kompilator.
 * @file: expr.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pro analyzator vyrazu.
 */
#ifndef _EXPR_H
#define _EXPR_H

/*
 * Vycet hodnot, ktere se daji ulozit na zasobnik (nektere se tam ale neukladaji)
 */

#define MAX_SERR    3                                   /* poèet moŸných chyb */
#define SERR_INIT   1                                  /* chyba pøi stackInit */
#define SERR_PUSH   2                                  /* chyba pøi stackPush */
#define SERR_TOP    3                                   /* chyba pøi stackTop */

#define MAX_STACK 100

extern int STACK_SIZE1;

enum treserved
{
  ERDO = 0,  /**< Vsechno je v poradku.              */
  ERELSE = 1,  /**< Chyba v ramci lexikalni analyzy.   */
  EREND = 2,  /**< Chyba v ramci syntakticke analyzy. */
  ERFALSE = 3,  /**< Chyba v ramci semanticke analyzy.  */
  ERFUNCTION = 4,  /**< Chyba v ramci interpretace.        */
  ERIF = 5,  /**< Chyba interni pri prekladu.        */
  ERLOCAL = 6,  /**< Chyba v ramci interpretace.        */
  ERNIL = 7,  /**< Chyba interni pri prekladu.        */
  ERREAD = 8,  /**< Chyba v ramci interpretace.        */
  ERRETURN = 9,  /**< Chyba interni pri prekladu.        */
  ERTHAN = 10,  /**< Chyba v ramci interpretace.        */
  ETRUE = 11,  /**< Chyba interni pri prekladu.        */
  ERWHILE = 12,  /**< Chyba v ramci interpretace.        */
  ERWRITE = 13,  /**< Chyba interni pri prekladu.        */
  ERAND = 14,  /**< Chyba v ramci interpretace.        */
  ERBREAK = 15,  /**< Chyba interni pri prekladu.        */
  ERELSEIF = 16,  /**< Chyba v ramci interpretace.        */
  ERFOR = 17,  /**< Chyba interni pri prekladu.        */
  ERIN = 18,  /**< Chyba v ramci interpretace.        */
  ERNOT = 19,  /**< Chyba interni pri prekladu.        */
  EROR = 20,  /**< Chyba v ramci interpretace.        */
  ERREPEAT = 21,  /**< Chyba interni pri prekladu.        */
  ERUNTIL = 22,  /**< Chyba v ramci interpretace.        */
  ERMAIN = 23,  /**< Chyba v ramci interpretace.        */
  ERSORT = 24,  /**< Chyba v ramci interpretace.        */
  ERTYPE = 25,  /**< Chyba v ramci interpretace.        */
  ERSUBSTR = 26,  /**< Chyba v ramci interpretace.        */
  ERFIND = 27,  /**< Chyba v ramci interpretace.        */
};

extern int err_flag;                   /* Indikuje, zda operace volala chybu. */

// Vstupni identifikatory
#define EPLUS     0      // +
#define EMINUS    1      // -
#define EMUL      2      // *
#define EDIV      3      // /
#define EPOW      4      // ^
#define ECONCAT   5      // ..
#define EEQUAL    6      // ==
#define ENEQUAL   7      // ~=
#define ELESS     8      // <
#define EGREAT    9      // >
#define ELEQUAL   10     // <=
#define EGEQUAL   11     // >=
#define EFUNC     12     // f
#define EID       13     // i
#define ELPARA    14     // (
#define ERPARA    15     // )
#define ECOMMA    16     // ,
#define ELAST     17     // ; , )
#define EMOD      18     // %
#define ESTRING   19     // S
#define EBOOL     20     // bool
#define ENUMBER   21     // Number
#define ENIL	  22     // NIL
#define EEXPR     30     // E
#define EREAD     40     // read
#define EWRITE    50     // write
#define EERROR    70     // error
#define EERRORSYN    80     // error-syn

// Vyctovy typ pro LR tabulku
enum PA{
        PaEq,   // =
        PaLs,   // <
        PaGr,   // >
        PaEm,   // empty field
} ;

// Vyctovy typ pro navratove hodnoty SA
enum RV{
        RVNull,          // Normal
        RVRead,          // read()
        RVWrite,         // write()
        RVComma,         // ,
        RVRPara,         // )
        RVFunc, 	 // function()
        RVIf,	         // if
        RVWhile,         // while
} ;

// Struktura pro zpracovani vyrazu
typedef struct {
        int type;        // Typ prvku tElement
	int lit;
        int error;       // Chybovy kod
        tData *data;     // Struktura obsahujici data pro binary tree
} tElement;

// Struktura pro zasobnik
typedef struct {
        int block;
        int top;
        tElement* arr;
} tStack;

void nastavStrom(BNode *node);
void deallocElements(tElement *token, tElement *tmp, tElement *var1, tElement *var2, tStack *stack, string *varName);
string genUniqueKey(string funcName);
string genUniqueKeyNumber(void);
void exprStackError ( int error_code );
void exprStackInit ( tStack* s);
int exprStackEmpty ( const tStack* s );
int exprStackFull ( const tStack* s );
void exprStackTopTerminal(const tStack* s,tElement* element);
void exprStackTopAll ( const tStack* s, tElement* element );
void exprStackPop ( tStack* s );
void exprStackPush ( tStack* s, tElement* element );
void exprStackDispose( tStack *s );
int nextToken (string * attr, int *func);
tElement* syntAnalExpr(int *func, string funcName, tListOfInstr *list);
void generateInstruction(int instType, void *addr1, void *addr2, void *addr3);

#endif
