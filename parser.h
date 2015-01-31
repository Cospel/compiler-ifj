/**
 * Projekt: IFJ11 kompilator.
 * @file: parser.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pro parser.
 */


#define SYN_ERROR 2
#define SYN_OK 0

// seznam obsahujici vsechny pouzite funkce a zda byly definovany
// pred spustenim interpretu se seznam projde a pokud bude obsahovat
// nedefinovane funkce, je to chyba
typedef struct funcListItem {
  string funcName;
  int isDef;
  struct funcListItem *nextItem;
  void *adresaFunkce;
  void *adresaNavratu;
} tFuncListItem;

typedef struct {
  struct funcListItem *First;
  struct funcListItem *Last;
} tFuncList;

int program(tListOfInstr *ilist, BNode *node);
int oFunction();
int mFunction();
int params(int *paramPocet);
int paramsN(int *paramPocet);
int body();
int varDecl();
int decl(tElement **pomData);
int statList();
int stat();
int assign();
int write();
int gRepeat();
int gWhile();
int gIf();
void generateInstruction(int instType, void *addr1, void *addr2, void *addr3);

void initFList(tFuncList *fList);
void disposeFList(tFuncList *fList);
tFuncListItem *insertFLast(tFuncList *fList, string fName, int def);
void printFuncList(tFuncList *fList);
tFuncListItem *updateFuncList(tFuncList *fList, string fName, int def);
