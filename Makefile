#
# Projekt: IFJ11 kompilator
# Author: Peter Michalik, xmicha47
#         Milan Seitler, xseitl01
#         Jakub Sznapka, xsznap01
#         Jan Hrivnak, xhrivn01
#         Michal Lukac, xlukac05
# Popis : Makefile pre projekt do predmetu IFJ na VUT FIT
#

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm
EXECUTE=ifj11
OBJECTS=str.o ilist.o ial.o scaner.o jumpstack.o expr.o parser.o interpret.o main.o
ZIPNAME=xmicha47

$(EXECUTE): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *\.o
pack:
	zip -r $(ZIPNAME).zip *.c *.h Makefile rozdeleni rozsireni dokumentace.pdf
