/**
 * Projekt: IFJ11 kompilator.
 * @file: interpret.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pre interpretaci.
 */

// ascii hodnoty
#define LOMITKO 92
#define UVOZOVKY 34
#define NOVYRADEK 10

/* Instrukcni sada */
enum tins {
  I_LAB_MAIN, //null,null,null      // navesti function main
  I_LAB,      //null,null,null      // navesti
  I_LAB_F,    //null,null,null      // navesti funkce konec pushovani promennych
  I_LAB_S,    //null,null,null      // navesti pro zacatek funkce
  I_SLENGTH,  //$1,null,$3          // $1 = length($3)
  I_MOD,      //$1,null,$3          // $1 = $2 % $3
  I_SUB,      //$1,$2,$3            // $1 = $2 - $3
  I_ADD,      //$1,$2,$3            // $1 = $2 + $3
  I_MUL,      //$1,$2,$3            // $1 = $2 * $3
  I_DIV,      //$1,$2,$3            // $1 = $2 / $3
  I_CONCAT,   //$1,$2,$3            // $1 = $2 .. $3
  I_POWER,    //$1,$2,$3            // $1 = $2 ^ $3
  I_EQUAL,    //$1,$2,$3            // $1 = $2 == $3
  I_NEQUAL,   //$1,$2,$3            // $1 = $2 ~= $3
  I_LCMP,     //$1,$2,$3            // $1 = $2 < $3
  I_GCMP,     //$1,$2,$3            // $1 = $2 > $3
  I_ELCMP,    //$1,$2,$3            // $1 = $2 <= $3
  I_EGCMP,    //$1,$2,$3            // $1 = $2 >= $3
  I_STRUE,    //$1,null,null        // $1 = $1 do dolar jedna vyhodnot vyraz $1 / nastavime ci je true ci nie
  I_JMPN,     //$1,null,$3          // skoc na $3 pokial $1 je false, inak neskakaj
  I_JUMP,     //$1,null,$3          // skoc na navestie $1, $3 je pre navrat z funkcie
  I_ASGN,     //$1,null,$3          // $1 = $3
  I_ASGNR,    //$1, null, null      // $1 = zapamatovana premenna z posledneho volani
  I_READ,     //$1,null,$3          // $1 = read($3)
  I_WRITE,    //$1,null,null        // vypis $1
  I_FTYPE,    //$1, null, $3        // $1 = type($3)
  I_FSUBS,    //null,null,null      // zavolej funkci substring parametre na zasobniku?
  I_FFIND,    //$1,$2,$3            // $1 = find($2,$3)
  I_FSORT,    //$1,null,$3          // $1 = sort($3)
  I_PUSH,     //$1,$2,null          // push na zasobnik ak je $1, alebo $2 pri pushu do parametru
  I_RETURN,   //$1,null,null        // zapamataj si ze vracas $1, zniz hodnotu zanoreni
  I_END,      //null,null,null      // konec programu
};

void makeCopy(tData *data, tData *copy);
int interpret(tListOfInstr *iList, BNode *btree);
void errorFree();
/*** Koniec souboru interpret.h ***/
