/**
 * Projekt: IFJ11 kompilator.
 * @file: scaner.c
 * @author: Peter Michalik,  xmicha47
 *          Milan Seitler, xseitl01
 *          Jakub Sznapka, xsznap01
 *          Jan Hrivnak, xhrivn01
 *          Michal Lukac, xlukac05
 * @brief: Kompilator jazyku IFJ11 do predmetu IFJ na VUT FIT 2011/2012.
 *         Soubor pro lexikalni analyzator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "str.h"
#include "scaner.h"

// promenna pro ulozeni vstupniho souboru
FILE *source;

void setSourceFile(FILE *f) { //nastaveni vstupniho souboru
    source = f;
}

int getNextToken(string *attr)
// hlavni funkce lexikalniho analyzatoru
{
    int state = 0; // cislo stavu KA
    int c; //precteny znak
    char lomeno='\\'; // lomitko, kvùli tisku do attr
    char esc_sekv[3]; //pole pro tri cisla kvùli prevodu \ddd na znak s ASCII hodnotou ddd

    /* vymazeme obsah atributu a v pripade identifikatoru nebo cisla
    do neho budeme postupne do nej vkladat jeho nazev */
    strClear(attr);

    while (1) {
        c = getc(source); // nacteni dalsiho znaku
        switch (state) {
            case 0: // zakladni stav automatu
                if (isspace(c)) { // bila mista - ignorovat
                    state = 0;
                } else if (c == '{') {
                    return LEVA_SLOZENA_ZAV;
                } else if (c == '}') {
                    return PRAVA_SLOZENA_ZAV;
                } else if (c == '(') {
                    return LEVA_NORMAL_ZAV;
                } else if (c == ')') {
                    return PRAVA_NORMAL_ZAV;
                } else if (c == ';') {
                    return STREDNIK;
                } else if (c == '+') {
                    return PLUS;
                } else if (c == '*') {
                    return KRAT;
                } else if (c == '/') {
                    return DELENO;
                } else if (c == '^') {
                    return MOCNINA;
                } else if (c == ',') {
                    return CARKA;
                } else if (c == '#') {
                    return LENGHT;
                } else if (c == '%') {
                    return MODULO;
                } else if (c == EOF) {
                    return END_OF_FILE;
                } else if (c == '=') { // = nebo ==
                    state = 1;
                } else if (c == '~') { //nerovno nebo error
                    state = 2;
                } else if (c == '<') { // < nebo <=
                    state = 3;
                } else if (c == '>') { // > nebo >=
                    state = 4;
                } else if ((isalpha(c)) || (c == '_')) { // identifikator
                    strAddChar(attr, c);
                    state = 5;
                } else if (isdigit(c)) { //cislo
                    strAddChar(attr, c);
                    state = 6;
                } else if (c == '"') { //retezec
                    state = 12;
                } else if (c == '-') { //minus nebo komentar
                    strAddChar(attr, c);
                    state = 14;
                } else if (c == '.') { //konkatenace nebo error
                    state = 20;
                } else if (c == EOF) {
                    return END_OF_FILE;
                } else {
                    return LEX_ERROR;
                }
                break;


            case 1:
                // = nebo ==
                if (c == '=') {
                    return POROVNANI;
                } else {
                    ungetc(c, source);
                    return ROVNASE;
                }
                break;


            case 2:
                if (c == '=') { // ~=
                    return NEROVNASE;
                } else {
                  return LEX_ERROR;
                }
                break;

            case 3:
                // <= nebo <
                if (c == '=') {
                    return MENSIROVNO;
                } else {
                  ungetc(c, source);
                  return MENSI;
                }
                break;

            case 4:
                // > nebo >=
                if (c == '=') {
                    return VETSIROVNO;
                } else {
                  ungetc(c, source);
                  return VETSI;
                }
                break;



            case 5:
                // identifikator nebo klicove slovo
                if ((isalnum(c)) || (isdigit(c)) || (c == '_')){
                    // identifikator pokracuje
                    strAddChar(attr, c);
                } else {
                    // konec identifikatoru
                    ungetc(c, source);
                    // kontrola, zda se nejedna o klicove slovo
                    if (strCmpConstStr(attr, "do") == 0) {
                        return DO;
                    } else if (strCmpConstStr(attr, "else") == 0) {
                        return ELSE;
                    } else if (strCmpConstStr(attr, "end") == 0) {
                        return END;
                    } else if (strCmpConstStr(attr, "false") == 0) {
                        return FALSE;
                    } else if (strCmpConstStr(attr, "function") == 0) {
                        return FUNCTION;
                    } else if (strCmpConstStr(attr, "if") == 0) {
                        return IF;
                    } else if (strCmpConstStr(attr, "local") == 0) {
                        return LOCAL;
                    } else if (strCmpConstStr(attr, "nil") == 0) {
                        return NIL;
                    } else if (strCmpConstStr(attr, "read") == 0) {
                        return READ;
                    } else if (strCmpConstStr(attr, "return") == 0) {
                        return RETURN;
                    } else if (strCmpConstStr(attr, "then") == 0) {
                        return THEN;
                    } else if (strCmpConstStr(attr, "true") == 0) {
                        return TRUE;
                    } else if (strCmpConstStr(attr, "while") == 0) {
                        return WHILE;
                    } else if (strCmpConstStr(attr, "write") == 0) {
                        return WRITE;
                    } else if (strCmpConstStr(attr, "and") == 0) {
                        return AND;
                    } else if (strCmpConstStr(attr, "break") == 0) {
                        return BREAK;
                    } else if (strCmpConstStr(attr, "elseif") == 0) {
                        return ELSEIF;
                    } else if (strCmpConstStr(attr, "for") == 0) {
                        return FOR;
                    } else if (strCmpConstStr(attr, "in") == 0) {
                        return IN;
                    } else if (strCmpConstStr(attr, "not") == 0) {
                        return NOT;
                    } else if (strCmpConstStr(attr, "or") == 0) {
                        return OR;
                    } else if (strCmpConstStr(attr, "repeat") == 0) {
                        return REPEAT;
                    } else if (strCmpConstStr(attr, "until") == 0) {
                        return UNTIL;
                    } else {
                        // jednalo se skutecne o identifikator
                        return ID;
                    }
                }
                break;


            case 6: //cisla
                if (isdigit(c)) { //zustavame ve stavu 6
                    strAddChar(attr, c);
                } else if(c == '.'){ //desetinna cast
                  //strAddChar(attr, c); //nemuzeme proidat tecku do retezce, muze to byt jeste konkatenace za cislem
                  state=7;
                } else if((c == 'e') || (c == 'E')){ //exponencialni cast
                  strAddChar(attr, c);
                  state=9;
                } else { //konec cisla
                  ungetc(c, source);
                  return CISLO;
                }
                break;


            case 7:
                if (isdigit(c)) { //zacatek desetinne casti
                    strAddChar(attr, '.');
                    strAddChar(attr, c);
                    state=8;
                } else { //za desetinou teckou neni cislo
                  return LEX_ERROR;
                }
                break;


            case 8:
                if (isdigit(c)) { //pokracovani desetinne casti
                  strAddChar(attr, c);
                } else if((c == 'e') || (c == 'E')){ //exponencialni cast
                  strAddChar(attr, c);
                  state=9;
                } else { //konec cisla
                  ungetc(c, source);
                  return CISLO;
                }
                break;


            case 9:
                if (isdigit(c)) { //exponent
                  strAddChar(attr, c);
                  state=11;
                } else if((c == '+') || (c == '-')){
                  strAddChar(attr, c);
                  state=10;
                } else {
                  return LEX_ERROR;
                }
                break;


            case 10:
                if(isdigit(c)){
                  strAddChar(attr, c);
                  state=11;
                }else { //za +- neni cislo
                  return LEX_ERROR;
                }
                break;


            case 11:
                if (isdigit(c)) { //exponent
                  strAddChar(attr, c);
                } else { //konec cisla s exponentem
                  ungetc(c, source);
                  return CISLO;
                }
                break;


            case 12:
                if(c == '"'){ //konec retezce
                  return TEXT_RETEZEC;
                }else if(c == '\\'){ //vstup do escape sekvence
                  state=13;
                }else{
                  if (c == EOF) { //neukonceny textovy retezec
                    return LEX_ERROR;
                  }
                  strAddChar(attr, c); //obsah retezce
                }
                break;

            case 13: // esc sekvence
                if (c == EOF) { //neukonceny textovy retezec
                  return LEX_ERROR;
                }
                esc_sekv[0]=c; // prvni znak esc sekvence
                if(isdigit(c)){
                  state=21; // moznost \ddd
                }else if((c == 'n') || (c == 't') || (c == '\\') || (c == '"')){
                  state=12; // jeden znak v esc sekvenci, vlozime ho do retezce
                  strAddChar(attr, lomeno);
                  strAddChar(attr, esc_sekv[0]);
                }else{
                  return LEX_ERROR;
                }
                break;


            case 21:
                if (c == EOF) {//neukonceny textovy retezec
                  return LEX_ERROR;
                }
                esc_sekv[1]=c;  // druhy znak esc sekvence
                if(isdigit(c)){
                    state=22; // ciselna esc sekvence
                } else { // esc sekvence s 1 cislem a jednou blbosti
                  return LEX_ERROR;
                }
                break;


            case 22:
                if (c == EOF) {//neukonceny textovy retezec
                  return LEX_ERROR;
                }
                esc_sekv[2]=c; // treti znak esc sekvence
                if(isdigit(c)){ // \ddd -spravna ciselna esc sekvence
                    state=12;
                    int cislo=atoi(esc_sekv);
                    if ((cislo<=255) && (cislo>0)){
                      char znak=cislo;
                      strAddChar(attr, znak);
                    }else{ // \999 neni povoleno
                      return LEX_ERROR;
                    }
                } else { // \11x treti znak neni cislo
                  return LEX_ERROR;
                }
                break;


            case 14:
                if (c == '-') { //komentar
                    strClear(attr);
                    state=15;
                } else { //minus
                  ungetc(c, source);
                  return MINUS;
                }
                break;


            case 15:
                if (c == '[') { // --[
                    state=16;
                } else if(c == '\n'){ //konec jednoradkoveho komentare
                  state=0;
                }else if (c == EOF) {
                  return END_OF_FILE;
                }else{
                  state=19;
                }
                break;


            case 16:
                if (c == '[') { //viceradkovy komentar
                    state=17;
                } else if(c == '\n'){ //konec jednoradkoveho komentare
                  state=0;
                }else if (c == EOF) {
                  return END_OF_FILE;
                }else{ //pokracovani jednoradkoveho komentare
                  state=19;
                }
                break;


            case 17:
                if (c == ']') { //prvni znak konce komentare
                    state=18;
                }else if (c == EOF) {
                  return LEX_ERROR;
                }
                break;


            case 18:
                if (c == ']') { //konec viceradkoveho komentare
                    state=0;
                }else if (c == EOF) {
                  return LEX_ERROR;
                }else{ //pokracovani viceradkoveho komentare
                  state=17;
                }
                break;


            case 19:
                if (c == '\n') { //konec jednoradkoveho komentare
                    state=0;
                }else if (c == EOF) {
                  return END_OF_FILE;
                }
                break;


            case 20:
                if (c == '.') { // konkatenace
                    return KONKATENACE;
                }else{
                  return LEX_ERROR;
                }
                break;
        } //switch
    }//while(1)
}
