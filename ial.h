/**
 * Projekt: IFJ11 kompilator.
 * @file: ial.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pro zakladni algoritmy.
 */

#ifndef _IAL_H
#define _IAL_H

#define HAVENT_VALUE 0
#define HAVE_VALUE 1

/* Datovy typy */
enum dtypes {
  T_NOTDEF = 0,               // Datovy typ neni definovan.
  T_NUMBER = 1,               // Cislo double.
  T_STRING,                   // Retezezec
  T_BOOLEAN,                  // Bool = TRUE/ FALSE
  T_NIL,                      // NIL
  T_FUNC,                     // Funkce.
  T_ID,                       // Identifikator slouzi k identifikaci u expr.
};

/* Data v btree stromu */
typedef struct{
  string varKey;              // klic pro vyhledani reprezentujici identifikator
  int hasValue;               // maju data inicializovanu hodnotu?
  int varType;                // typ pro promennou
  int prmCount;               // pocet parametru ak je typom funkce
  union {                     // UNION jeden z nasledujicich typu
    double dValue;            // double hodnota
    string sValue;            // string retezec
    int bValue;               // bool hodnota nebo nil = 0
  } value;
} tData;

/* Uzel btree stromu */
typedef struct node
{
  tData data;                 // data v uzly
  struct node *right, *left;  // lavy a pravy uzel
} *BNode;

// Pole pro vzorky potrebne pro Boyer Moore vyhledavaci algoritmus.
int CharJump[256];


// Funkce boyer moore
int boyerMooreSearch(string *s1,string *s2);
void computeCharJump(string *s);

// Funkce pre btree strom
// je vice moznosti pridavani do stromu a hledani v binarnim stromu
//void initTree(BNode *tree);
BNode makeBtreeNode(string *s1);
void leftBtree(BNode *l,string *s1);
void rightBtree(BNode *r,string *s1);
void destroyTree(BNode *node);
tData *searchBtreeStr(BNode *root, string *s1);
tData *searchBtreeCharStr(BNode *root, char *s1);
int insertDataBtree(BNode *root, string *s1, tData *data);
int insertBtree(BNode *root, string *s1, int varType);
void fillNode(BNode *node, tData *data);
tData *getInsertBtree(BNode *root, string *s1, tData *data);
void initTree(BNode *node);

// Funkce pro mergesort
void sort(string *input,string *result);
void mergesort(string *result, string *tmp, int low, int high);
void merge(string *result, string *tmp, int low, int middle, int high);

#endif // IAL_H
/*** Konec souboru ial.h ***/
