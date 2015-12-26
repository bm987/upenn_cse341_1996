%{

/*
 * $Id: mcc.l,v 1.1 1996/09/17 21:15:00 berrym Exp $
 * $Log: mcc.l,v $
 * Revision 1.1  1996/09/17  21:15:00  berrym
 * Initial revision
 *
 * Revision 1.2  1996/04/08  06:46:52  berrym
 * incorporated with parser; first working package
 * up to parsing declarations, not error free
 *
 * Revision 1.1  1996/02/23  14:36:49  berrym
 * Initial revision
 *
 * mcc -- lexer
 *
 * Mike Berry
 * University of Pennsylvania
 * 23 Feb 1996
 */

#include "globals.h"
#include "mcc.tab.h"

#define do_return(token) if (token_listing) printf ("token: : (%d) %s\n",token,yytext); return (token)

extern int lookups;
extern int lookup_type;
extern int linear_jumps;
extern int sym_debug;

extern struct table * symtab_l;	        /* global symtab list ptr */
extern struct table * symtab_cur;	/* global current scope pointer */
extern struct t_node * tree_root;	/* global tree root of syntax tree */
struct entry * ptr;		/* tmp ptr */

int token_listing = 0;		/* list each token? */
int ret;			/* token return value */

%}

				/* these defs were borrowed from the cse341 */
				/* newsgroup after i did my own version and */
				/* handed it in 2/20/96 */

single  [;,:&=\+\-\*\/\%\!\(\)\{\}\[\]\.\<\>]
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z]
digit   [0-9]
id      {letter}({letter}|{digit}|_)*
digits  {digit}+
onum    0[0-7]*
nonum   0[0-7]*[89]+{digit}*
hexnum  0[xX][0-9a-fA-F]+
inum    ([1-9]{digit}*)
special \\[ntvbrfa\\\?\'\"]
chars   '({special}|[^'\n])'
strings \"({special}|[^\\\"\n]|\\\n)*\"
fnum    (({digit}*\.{digits})|({digits}\.))([eE][+-]?{inum})?|({digits}[eE][+-]?{inum})


%x COMMENTS

%%

^#.*\n {     if (strchr(yytext,'\n'))
	  yylineno++;
}

{ws}    { 
     if (strchr(yytext,'\n'))
	  yylineno++;
}


\/\*debug\(token_listing_on\) { 
     BEGIN(COMMENTS);
     token_listing = 1;
}

\/\*debug\(token_listing_off\) { 
     BEGIN(COMMENTS);
     token_listing = 0;
}

\/\*debug\(sym_debug_on\) { 
     BEGIN(COMMENTS);
     sym_debug = 1;
}

\/\*debug\(sym_debug_off\) { 
     BEGIN(COMMENTS);
     sym_debug = 0;
}

\/\*debug\(symtab_dump\) { 
     BEGIN(COMMENTS);
     symtab_dump(symtab_cur);
}

\/\*debug\(symtab_stats\) { 
     BEGIN(COMMENTS);
     symtab_stats(symtab_cur);
}

\/\*debug\(statement_dump\) { 
     BEGIN(COMMENTS);
     dump_tree(tree_root, 0);
}



\/\*debug\(halt\) { 
     BEGIN(COMMENTS); 
     exit (0);
}

\/\* {
     BEGIN(COMMENTS); 
}

<COMMENTS>[^\*]* ;

<COMMENTS>\*\/ {
     BEGIN(INITIAL); 

}

<COMMENTS>. ;

char 		do_return( CHAR );
extern     	do_return( EXTERN );
return      	do_return( RETURN );
do      	do_return( DO );
for      	do_return( FOR );
void      	do_return( VOID );
float 		do_return( FLOAT );
if     		do_return( IF );
while      	do_return( WHILE );
else      	do_return( ELSE );
int      	do_return( INT );
register      	do_return( REGISTER );

"->" do_return( PTR );
"==" do_return( EQ );
"!=" do_return( NEQ );
"<=" do_return( LEQ );
"&&" do_return( AND );
"++" do_return( PLUS );
">=" do_return( GEQ );
"||" do_return( OR );
"--" do_return( MINUS );

{single}  {
     do_return( (*yytext) );}

{id}  { 
     if (!(yylval.sym=lookup(symtab_cur, yytext)))
     {
	  yylval.sym = insert(symtab_cur,creat_symbol(yytext, 1));  
     }

     do_return( ID );
}

{inum}|{onum}|{hexnum} {
     if (!(yylval.sym=lookup(symtab_cur, yytext)))
     {
	  yylval.sym = insert(symtab_cur,creat_symbol(yytext, 1));  
     }

     yylval.sym->type=t_int|c_scalar|e_const;
     do_return( ICON );
}
{fnum} { 
     if (!(yylval.sym=lookup(symtab_cur, yytext)))
     {
	  yylval.sym = insert(symtab_cur,creat_symbol(yytext, 1));  
     }
     yylval.sym->type=t_float|c_scalar|e_const;
     do_return( FCON );
}
{chars} {
     if (strchr(yytext,'\n'))
	  yylineno++;

     *(yytext) = '\'';
     *(yytext+2) = '\'';
     if (!(yylval.sym=lookup(symtab_cur, yytext)))
     {
	  yylval.sym = insert(symtab_cur,creat_symbol(yytext, 1));  
     }
     yylval.sym->type=t_char|c_scalar|e_const;
     do_return( CCON );
}

{strings} {
     if (strchr(yytext,'\n'))
	  yylineno++;

    /* add check here */
     ptr = creat_symbol(yytext, 0);
     ptr->type = e_const | t_char | c_array;
     yylval.sym = insert(symtab_cur,ptr);
     do_return( SCON );
}
{nonum}|. do_return( ILLEGAL );

%%

/*
 * main() is now in the parser
 */


/*
main ()
{
     extern void parse_main();
     int ret;
     parse_main();
     while ((ret = yylex())) 
     {
	  if (token_listing)
	  {
	       printf ("%d: ",yylineno);
		    printf ("<%s, >\n",yytext);
	  }
     }
}

*/
