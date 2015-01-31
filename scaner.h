/**
 * Projekt: IFJ11 kompilator.
 * @file: scanner.h
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Hlavickovy soubor pre scaner.c.
 */
#ifndef _SCANER_H
#define _SCANER_H


#define CISLO   2         //cislo v libovolnem tvaru
#define TEXT_RETEZEC   3  //textovy retezec
#define ID      4         //identifikator

//klicova slova
#define DO      10
#define ELSE    11
#define END     12
#define FALSE   13
#define FUNCTION 14
#define IF      15
#define LOCAL   16
#define NIL     17
#define READ    18
#define RETURN  19
#define THEN    20
#define TRUE    21
#define WHILE   22
#define WRITE   23

//rezervovana slova
#define AND     24
#define BREAK   25
#define ELSEIF  26
#define FOR     27
#define IN      28
#define NOT     29
#define OR      30
#define REPEAT  31
#define UNTIL   32

//jednotlive znaky
#define LEVA_SLOZENA_ZAV 40  // '{'
#define PRAVA_SLOZENA_ZAV 41 // '}'
#define LEVA_NORMAL_ZAV 42   // '('
#define PRAVA_NORMAL_ZAV 43  // ')'
#define STREDNIK         44  // ';'
#define ROVNASE          45  // '='
#define NEROVNASE        46  // '~='
#define POROVNANI        47  // '=='
#define PLUS            48   // '+'
#define MINUS           49   // '-'
#define KRAT            50   // '*'
#define DELENO          51   // '/'
#define MOCNINA         52   // '^'
#define MENSI           53   // '<'
#define VETSI           54   // '>'
#define MENSIROVNO      55   // '<='
#define VETSIROVNO      56   // '>='
#define KONKATENACE     57   // '..'
#define CARKA           58   // ','
#define LENGHT          59   // '#'
#define MODULO          60   // '%'

//specialni znaky
#define END_OF_FILE    70

//chybove hlasky
#define LEX_ERROR    1

//hlavicka funkce simulujici lexikalni analyzator
int getNextToken(string *attr);
void setSourceFile(FILE *f);

#endif
