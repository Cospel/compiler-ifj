/**
 * Projekt: IFJ11 kompilator.
 * @file: ial.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro zakladni algoritmy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "ial.h"

/**
 * Funkce boyerMooreSearch pro vyhledani podretezce v retezci.
 * @param s1 Retezec.
 * @param s2 Podretezec.
 * @return Pozice najdeni.
 */
int boyerMooreSearch(string *s1,string *s2)
{
  computeCharJump(s2);  // vypocteme si pole pro skoky
  int j = s2->length;   // zacneme hledat az od konca hledanyho retezce
  int k = s2->length;   // k je index na konec hledany retezce
  int max = s2->length; // maximalni hodnota indexu

  // BOYER - MOORE algoritmus
  while(j <= s1->length && k > 0)
  {
    // Je posledni znak  == text[znak.dlzka]
    if(s1->str[j-1] == s2->str[k-1])
    {
      j = j -1;
      k = k -1;
    }
    // Inak nastav novy indexy
    else
    {
      // Index J je index do prohledavanyho textu.
      if(j == j + CharJump[(int)(s1->str[j-1])] || max == j + CharJump[(int)(s1->str[j-1])])
        j = max + s2->length; // + 1
      else
        j = j + CharJump[(int)(s1->str[j-1])];
      if(max < j) max = j; // Nastaveni max prvku jelikoz vyuzivame jenom jednu heuristiku.
      k = s2->length;      // Index hledany retezce si obnovy hodnotu na koniec.
    }
  }

  if(k == 0)
    return j + 1;

  return 0;
}

/**
 * Vypocet pola pro skoky. Prvni heuristika : skripta IAL.
 * @param s Retezec ktory sa bude hledat v textu.
 */
void computeCharJump(string *s)
{
  // projdeme ASCII tabulku a inicializujeme pole CharJump
  for(int i = 0; i < 256; i++)
    CharJump[i] = s->length - 1;

  // projdeme retezec a do CharJump vlozime hodnoty pro skoky
  for(int i = 0; i < s->length; ++i)
    CharJump[(int)(s->str[i])] = s->length -1 - i;
}

/**
 * Funkce initTree inicializuje binarni strom.
 * @param node je korenovy uzol stromu.
 */
void initTree(BNode *node)
{
  *node = NULL;
}

/**
 * Rusi postupne cely strom od zadanyho korene.
 * @param node je uzel stromu.
 */
void destroyTree(BNode *node)
{
  if((*node) != NULL)
  {
    // koukni se do levy a pravy casti uzla, rekurze
    destroyTree(&(*node)->left);
    destroyTree(&(*node)->right);

    // uvolni retezec-kluc a uzel
    strFree(&(*node)->data.varKey);

    // uvolni retezec ak byla promenna retezec
    if((*node)->data.varType == T_STRING || (*node)->data.varType == T_FUNC)
      strFree(&(*node)->data.value.sValue);

    // uvolni uzol
    free(*node);
    (*node) = NULL;
  }
}

/**
 * Vytvor uzel binarniho stromu a vrat ho.
 * @param newnode odkaz na novy uzel.
 * @param s1 retezec pro klic noveho uzla.
 * @return uspesnost funkce.
 */
BNode makeBtreeNode(string *s1)
{
  // alokuje misto pro uzel
  BNode newnode;
  if((newnode = malloc(sizeof(struct node))) == NULL)
    return NULL;

  // inicializuj kluc a nastav ukazatele na lavo a pravo na NULL
  strInitText(&(newnode->data.varKey),s1->str);
  newnode->right = NULL;
  newnode->left = NULL;

  // vrat vytvoreny uzel
  return newnode;
}

/**
 * Koukne se do levy casti uzla binarniho stromu.
 * @param r Uzol btree.
 * @param s1 retezec pro klic novyho uzlu.
 */
void leftBtree(BNode *node,string *s1)
{
  // vytvor uzel na levy strane
  if((*node)->left == NULL)
    (*node)->left = makeBtreeNode(s1);
}

/**
 * Koukne se do pravy casti uzla binarniho stromu.
 * @param r Uzol btree.
 * @param s1 retezec pro klic novyho uzlu.
 */
void rightBtree(BNode *node,string *s1)
{
  // vytvor uzel na pravy strane
  if((*node)->right == NULL)
    (*node)->right = makeBtreeNode(s1);
}

/**
 * Naplni uzel binarniho stromu.
 * @param node Uzol binarniho stromu.
 * @param data data pro vlozeni.
 */
void fillNode(BNode *node, tData *data)
{
  (*node)->data.varType = data->varType;
  (*node)->data.hasValue = data->hasValue;

  if(data->varType == T_STRING || data->varType == T_FUNC)
    strInit(&(*node)->data.value.sValue);

  if(data->hasValue)
  {
    if(data->varType == T_STRING)
      strAddText(&(*node)->data.value.sValue,(data->value.sValue.str));
    else if(data->varType == T_NUMBER)
      (*node)->data.value.dValue = data->value.dValue;
    else if(data->varType == T_BOOLEAN)
      (*node)->data.value.bValue = data->value.bValue;
    else if(data->varType == T_FUNC)
    {
      (*node)->data.prmCount = data->prmCount;
      strAddText(&(*node)->data.value.sValue,data->value.sValue.str);
    }
    else if(data->varType == T_NIL)
      (*node)->data.value.bValue = 0;
  }
}

/**
 * Funkce vlozi polozku do binarniho stromu.
 * @param node koren binarniho stromu.
 * @param s1 nazev identifikatoru.
 * @param data data pro pridani do uzlu.
 * @return data v pridanej polozke.
 */
tData *getInsertBtree(BNode *root, string *s1, tData *data)
{
  // Dva ukazatele.
  BNode p = *root;
  BNode q = *root;

  // Pokial jeste nebyl vytvoren zadny uzel.
  if((*root) == NULL)
  {
    (*root) = makeBtreeNode(s1);

    // vloz do node data
    fillNode(root, data);
    return &(*root)->data;
  }

  // Inak prohledej binarni strom.
  while(q != NULL)
  {
    p = q;
    if(strcmp(s1->str,p->data.varKey.str) < 0)
      q = p->left;
    else
      q = p->right;
  }

  // Pokial je retezec mensi chod na lavo inak do prava a vytvor dalsi uzel
  if(strcmp(s1->str,p->data.varKey.str) < 0)
  {
    leftBtree(&p,s1);
    fillNode(&(p)->left, data);
    return &(p->left->data);
  }
  else
  {
    rightBtree(&p,s1);
    fillNode(&(p)->right, data);
    return &(p->right->data);
  }
  return NULL;
}

/**
 * Funkcia insertBtree vlozi prvok do binarniho stromu.
 * @param root je koren binarniho stromu
 * @param s1 je retezec/kluc do binarniho stromu
 * @param data su data pro pridani do uzlu.
 */
int insertDataBtree(BNode *root, string *s1, tData *data)
{
  // Dva ukazatele.
  BNode p = *root;
  BNode q = *root;

  // Pokial jeste nebyl vytvoren zadny uzel.
  if((*root) == NULL)
  {
    (*root) = makeBtreeNode(s1);
    // vloz do node data
    fillNode(root, data);
    return 0;
  }

  // Inak prohledej binarni strom.
  while(q != NULL)
  {
    p = q;
    if(strcmp(s1->str,p->data.varKey.str) < 0)
      q = p->left;
    else
      q = p->right;
  }

  // Pokial je retezec mensi chod na lavo inak do prava a vytvor dalsi uzel
  if(strcmp(s1->str,p->data.varKey.str) < 0)
  {
    leftBtree(&p,s1);
    fillNode(&(p)->left, data);
  }
  else
  {
    rightBtree(&p,s1);
    fillNode(&(p)->right, data);
  }
  return 0;
}

/**
 * Funkcia insertBtree vlozi prvok do binarniho stromu.
 * @param root je koren binarniho stromu
 * @param s1 je retezec/kluc do binarniho stromu
 * @param varType typ promenne.
 */
int insertBtree(BNode *root, string *s1, int varType)
{
  // Dva ukazatele.
  BNode p = *root;
  BNode q = *root;

  // Pokial jeste nebyl vytvoren zadny uzel.
  if((*root) == NULL)
  {
    (*root) = makeBtreeNode(s1);
    (*root)->data.varType = varType;

    // ak je retezec inicializuj ho
    if(varType == T_STRING)
      strInit(&(*root)->data.varKey);

    return 0;
  }

  // Inak prohledej binarni strom.
  while(q != NULL)
  {
    p = q;
    if(strcmp(s1->str,p->data.varKey.str) < 0)
      q = p->left;
    else
      q = p->right;
  }

  // Pokial je retezec mensi chod na lavo inak do prava a vytvor dalsi uzel
  if(strcmp(s1->str,p->data.varKey.str) < 0)
  {
    leftBtree(&p,s1);
    p->left->data.varType = varType;
    // ak je retezec inicializuj ho
    if(varType == T_STRING)
      strInit(&p->left->data.varKey);
  }
  else
  {
    rightBtree(&p,s1);
    p->right->data.varType = varType;
    // ak je retezec inicializuj ho
    if(varType == T_STRING)
      strInit(&p->right->data.varKey);
  }
  return 0;
}

/**
 * Prohleda binarni strom.
 * @param root je koren binarniho stromu.
 * @param s1 je retezec podle kteryho se prohledava strom.
 * @return data v uzlu nebo NULL
 */
tData *searchBtreeStr(BNode *root, string *s1)
{
  BNode p = *root;
  BNode q = *root;

  if(q == NULL)
    return NULL;

  // Ak tam je pouze koren
  if(q->left == NULL && q->right == NULL && q != NULL)
  {
    if(strcmp(s1->str,q->data.varKey.str) == 0)
      return &(q->data);
  }

  while(q->left != NULL || q->right != NULL)
  {
    if(p == NULL)
      return NULL;

    // Podla toho ci je retezec mensi,vetsi, rovno se rozhodni pro posun.
    if(strcmp(s1->str,p->data.varKey.str) == 0)
      q = p;
    else if(strcmp(s1->str,p->data.varKey.str) < 0)
      q = p->left;
    else
      q = p->right;

    if(q == NULL)
      return NULL;

    // Ak sme nasli uzel, vrat data.
    if(strcmp(s1->str, q->data.varKey.str) == 0)
      return &(q->data);
    p = q;
  }
  return NULL;
}

/**
 * Prohleda binarni strom.
 * @param root je koren binarniho stromu.
 * @param s1 je retezec podle kteryho se prohledava strom.
 * @return data v uzlu nebo NULL
 */
tData *searchBtreeCharStr(BNode *root, char *s1)
{
  BNode p = *root;
  BNode q = *root;

  if(q == NULL)
    return NULL;

  // Ak tam je pouze koren
  if(q->left == NULL && q->right == NULL && q != NULL)
  {
    if(strcmp(s1,q->data.varKey.str) == 0)
      return &(q->data); 
  }

  while(q->left != NULL || q->right != NULL)
  {
    if(p == NULL)
      return NULL;

    // Podla toho ci je retezec mensi,vetsi, rovno se rozhodni pro posun.
    if(strcmp(s1,p->data.varKey.str) == 0)
      q = p;
    else if(strcmp(s1,p->data.varKey.str) < 0)
      q = p->left;
    else
      q = p->right;

    if(q == NULL)
      return NULL;

    // Ak sme nasli uzel, vrat data.
    if(strcmp(s1, q->data.varKey.str) == 0)
      return &(q->data);
    p = q;
  }
  return NULL;
}

/**
 * Zoradi vstupny retazec
 * @param string je vstupny retazec
 * @return adresu zoradeneho retazca
 */
void sort(string *input,string *result)
{
  // vytvorenie a alokovanie docasneho pola
  string temp;
  strInit(&temp);

  // prekopirovanie vstupneho retazca do vystupneho
  strCopyString(result, input);

  // prekopirovanie vstupneho retazca do pomocneho
  strCopyString(&temp, input);

  // pre potreby radenia potrebujeme index posledneho znaku
  int length = input->length-1;

  // radiaca funkcia
  mergesort(result, &temp, 0, length);

  // uvolnenie docasneho pola
  strFree(&temp);
}

/**
 * Rekurzivna funkcia deliaca retazec na jednotlive ciastky
 * @param result vystupny retazec
 * @param tmp pomocny retazec
 * @param low spodna hranica ciastkoveho retazca
 * @param high horna hranica cistkoveho retazca
 */
void mergesort(string *result, string *tmp, int low, int high)
{
  int middle;

  // funkcia sa vola rekurzivne, pokial je skumany usek aspon dlzky 1
  if (low < high)
  {
    // vypocita sa stred podla ktoreho sa bude delit
    middle = (low + high) / 2;

    // rekurzivne zavolame deliacu funkciu pre obe rozdelene polovice
    mergesort(result, tmp, low, middle);
    mergesort(result, tmp, middle+1, high);

    // zoradime rozdelene polovice
    merge(result, tmp, low, middle, high);
  }
}

/**
 * Funkcia vykonavajuca samotne radenie
 * @param result vysledny retazec
 * @param tmp pomocny retazec
 * @param low spodna hranica radenej casti
 * @param middle stredna hranica radenej casti
 * @param high horna hranica radenej casti
 */
void merge(string *result, string *tmp, int low, int middle, int high)
{
  int i, j, k;

  // prekopirovanie vstupneho retazca do pomocneho
  for (i = low; i<=high; i++)
    tmp->str[i] = result->str[i];

  i = low;
  j = middle + 1;
  k = low;

  // prechadzaj obe porovnavane casti (spodnu aj hornu)
  while (i<= middle && j<=high)
    // porovnaj ich a mensiu prirad do vysledneho retazca
    if (tmp->str[i] <= tmp->str[j])
    {
      result->str[k] = tmp->str[i];
      k++;
      i++;
    }
    else
    {
      result->str[k] = tmp->str[j];
      k++;
      j++;
    }

  // ak sme nedosiahli koniec prvej casti (bola vacsia ako druha cast), tak ju pripis k vysledku
  while (i<=middle)
  {
    result->str[k] = tmp->str[i];
    k++;
    i++;
  }
}
/*** Konec souboru ial.c ***/
