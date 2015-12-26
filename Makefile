# 
# $Id: Makefile,v 1.3 1996/09/17 21:14:25 berrym Exp $
# $Log: Makefile,v $
# Revision 1.3  1996/09/17  21:14:25  berrym
# Final submission.  Working version.
#
# Revision 1.2  1996/04/08  06:49:56  berrym
# new Makefile for use with parser
#
#
# Makefile for mcc
#
# Mike Berry
# 23 Feb 1996
#
	    
#CFLAGS = -g -DDEBUG
CFLAGS = -g
LEX = /usr/local/bin/flex
LFLAGS =
YACC = bison
YFLAGS =
LIBS = -lfl -ly
CC = gcc $(CFLAGS)

mcc: lex.yy.o symbol.o mcc.tab.o tree.o register.o
	$(CC)  -o mcc symbol.o lex.yy.o mcc.tab.o tree.o register.o -lfl -ly

mcc.tab.o: mcc.tab.h
	$(CC)  mcc.tab.c -c

mcc.tab.h: mcc.y globals.h
	$(YACC) -v -d mcc.y
	
lex.yy.o: mcc.l globals.h mcc.tab.h symbol.o tree.o
	$(LEX) mcc.l
	$(CC)  lex.yy.c -c

symbol.o: symbol.c globals.h
	$(CC)  symbol.c -c

register.o: register.c globals.h mcc.tab.h
	$(CC)  register.c -c

tree.o: tree.c globals.h mcc.tab.h
	$(CC)  tree.c -c

clean:
	rm -f *.o *~ \#* *.new mcc.tab.h mcc.tab.c lex.yy.c mcc.output mcc

