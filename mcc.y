%{

/*
 * $id: mcc.y,v 1.1 1996/04/08 06:48:45 berrym Exp berrym $
 * $Log: mcc.y,v $
 * Revision 1.3  1996/09/18  00:55:54  berrym
 * bug: no offset for addrof.  fixed.
 *
 * Revision 1.2  1996/09/17  21:14:37  berrym
 * Final submission.  Working version.
 *
 * Revision 1.1  1996/04/08  06:48:45  berrym
 * Initial revision
 *
 *
 * mcc -- parser routines
 *
 * bison rules for parsing mcc and constructing syntax trees.
 * type checking is done here and in tree.c
 *
 * includes main()
 *
 * Mike Berry
 * University of Pennsylvania
 * 8 Apr 1996
 */

#include "globals.h"

int set_type=0;
int set_exreg=0;

int lookups;
int linear_jumps;
int sym_debug;
extern int yylineno;
int zero = 0;

HalfWord glob_return_type;	/* remember what return type is of */
				/* current function */


struct entry *tmp_symbol;
struct t_node *tmp_node;


%}


%union {
     float fl;
     int in;
     char *ch;
     struct entry *sym;
     struct leaf *lf;
     struct t_node *t_n;
}

%token <sym> FCON
%token <sym> ICON CCON
%token <sym> SCON ID 
%token <in> FOR CHAR EXTERN RETURN
%token <in> STRUCT DO VOID DOUBLE FLOAT IF WHILE
%token <in> ELSE INT REGISTER PTR EQ
%token <in> NEQ GEQ LEQ OR AND PLUS MINUS ILLEGAL
%token <in> DEREF CALL ITOF X4 FTOI ADDROF CTOI CTOF ITOP CTOP ARYNDX FTOC ITOC
%token <in> '!' '=' '+' '-' '*' '/' '%' '<' '>' '?' '&' '[' ']'  '(' ')'

%left '+' '-'
%left '*' '/' 
%left OR AND
%left EQ NEQ GEQ '<' '>' LEQ 


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%type <in> unary_op
%type <sym> var_list var_item array_var scalar_var
%type <sym> decl_sub1 decl_sub2 
%type <in> modifier type_name
%type <sym> parm_type_list parm_list parm_decl_mid parm_decl internal_decls declaration
%type <t_n> statement stat_mid return_stmt expression unary_expr binary_expr
%type <t_n>  postfix_expr_root argument_list function_body function_def
%type <t_n> if_stmt
%type <t_n> for_stmt
%type <t_n> dowhile_stmt while_stmt
%type <t_n> statement_list compound_stmt /* tree_list */
%type <t_n> primary_expr constant postfix_expr
%type <sym> function_hdr


%%
 
program      : external_decls
             ;


external_decls : declaration external_decls {}
               | function_def { tree_root = $1; }
               ;

declaration : decl_sub1 
            ;

decl_sub1 : modifier decl_sub2
              {		   
		   tmp_symbol=$2;
		   while (tmp_symbol) 
		   { 
			SET_BITS(tmp_symbol->type,$1,ex_set); 
			tmp_symbol=tmp_symbol->next_par; 
		   }
		   $$=$2;
	      }
          | decl_sub2
          ;

decl_sub2 : type_name var_list ';' 
              {
		   tmp_symbol=$2;
		   while (tmp_symbol) 
		   { 
			SET_BITS(tmp_symbol->type,$1,t_set);
			tmp_symbol->offset = add_offset(tmp_symbol->type,
							tmp_symbol->size,
							symtab_cur, 0);
			tmp_symbol=tmp_symbol->next_par; 
		   }
		   $$=$2;
              }
            ;

modifier : EXTERN
             {
		  $$=ex_extern;
	     }
         | REGISTER
             { 
		  $$=ex_register;
	     }
         ;

type_name : VOID
              { 
		   $$ = t_void;
	      }
          | INT 
              {
		   $$ = t_int;
	      }
          | CHAR 
              {
		   $$ = t_char;
	      }
          | FLOAT 
              {
		   $$ = t_float;
	      }
          ;

var_list : var_item 
         | var_list ',' var_item
           {
		tmp_symbol=$1;
		while ($1)
		{
		     if ($1->next_par)
			  $1=$1->next_par;
		     else
			  break;
		}
		$1->next_par=$3;
		$$=tmp_symbol;
	   }
         ;

var_item : scalar_var 
           | array_var 
           | '*' scalar_var
	   {
		SET_BITS($2->type,c_pointer,c_set);
		$$=$2;
	   }
	   ;

jump_scope : { 
		symtab_cur = creat_symtab(symtab_l); 
}

jump_scope_norm : { 
		symtab_cur = creat_symtab(symtab_l); 
}


array_var : ID '[' ICON ']'
              {
		   /*
		    * force a local lookup and check for
		    * previous declarations of same name
		    */

		   if (!dectest(&$1, symtab_cur))
			yyerror("multiply defined identifier");

		   SET_BITS($1->type,c_pointer,c_set);
		   SET_BITS($1->type,e_var,e_set);
		   $1->size = atoi($3->name);
		   $$ = $1;
		   
				/* this raises an issue: shoudl you init */
				/* ID's with 0 or 1 refernces;;; right now */
				/* we're initing with 0 */

		   $3->references = -1; /* pretend this symbol doesn't */
				       /* exist, because it doesn't really */
				       /* need to be in the symtab */

	      }
          ;

scalar_var : ID 
               {
		   /*
		    * force a local lookup and check for
		    * previous declarations of same name
		    */

		   dectest(&$1, symtab_cur);

		    SET_BITS($1->type,c_scalar,c_set);
		    SET_BITS($1->type,e_var,e_set);
		    $$ = $1;    
	       }
           | ID '(' jump_scope parm_type_list ')'
               {
		    symtab_cur=pop_symtab(symtab_cur,0);

		    /*
		     * force a local lookup and check for
		     * previous declarations of same name
		     */
		    
		    dectest(&$1, symtab_cur);
		    
		    SET_BITS($1->type,c_scalar,c_set);
		    SET_BITS($1->type,e_fn,e_set);
		    $1->next_fn = $4;
		    $$ = $1;    

	       }
           ;

function_def : function_hdr '{' function_body '}' { 
		$$=$3;  /* this might change */
		symtab_cur=pop_symtab(symtab_cur,0);
		
 		}
	     ;	
function_hdr : type_name '*' ID '(' jump_scope parm_type_list ')' 
                {
		    /*
		     * force a local lookup and check for
		     * previous declarations of same name
		     */

		     SET_BITS($3->type,$1,t_set);
		    SET_BITS($3->type,e_fn,e_set);		     
		    SET_BITS($3->type,c_pointer,c_set);		     
		    glob_return_type = c_pointer|$1|e_fn;
		    dectest(&$3, symtab_cur->prev);

		    $3->next_fn = $6;
		    $$=$3;
		    glob_mainfn = $3; /* record main function name */
	       }

             | type_name ID '(' jump_scope   parm_type_list ')' 
               {
		    /*
		     * force a local lookup and check for
		     * previous declarations of same name
		     */

		     SET_BITS($2->type,$1,t_set);
		    SET_BITS($2->type,e_fn,e_set);		     
		    SET_BITS($2->type,c_scalar,c_set);		     
		    glob_return_type = c_scalar|$1|e_fn;
		    dectest(&$2, symtab_cur->prev);

		    $2->next_fn = $5;
		    glob_mainfn = $2; /* record main function name */

		    $$=$2;
	       }
             | ID '(' jump_scope  parm_type_list  ')' 
               { 
		    /*
		     * force a local lookup and check for
		     * previous declarations of same 
		     */
		    SET_BITS($1->type,e_fn,e_set);		     
		    SET_BITS($1->type,t_void,t_set);		     


		    glob_return_type = t_void|c_scalar|e_fn;
		    dectest(&$1, symtab_cur->prev);

		    $1->next_fn = $4;
		    glob_mainfn = $1; /* record main function name */
		    $$=$1;
	       }
	     ;

parm_type_list : parm_list { $$ = $1; glob_parm_offset = 68; } /* reset offset */
               | { $$ = (struct entry *) NULL; }
               ;

parm_list    : parm_decl_mid
             | parm_list ',' parm_decl_mid
                 {
		      tmp_symbol=$1;
		      while ($1)
		      {
			   if ($1->next_fn) 
				$1=$1->next_fn;
			   else
				break;
		      }
		      $1->next_fn=$3;
		      $$=tmp_symbol;
		 }    
             ;

parm_decl_mid : parm_decl {
                   HalfWord t1;

                   $$ = $1;
		   $$->offset = glob_parm_offset;
		   glob_parm_offset = glob_parm_offset+4; /* 4 for */
							  /* all types */
	      }

parm_decl : type_name ID 
              {
		    /*
		     * force a local lookup and check for
		     * previous declarations of same name
		     */
		    
		    dectest(&$2, symtab_cur);

		   SET_BITS($2->type,c_scalar,c_set);
		   SET_BITS($2->type,e_var,e_set);
		   SET_BITS($2->type,$1,t_set);
		   $$=$2;
	      }
          | type_name '*' ID
              {
		    /*
		     * force a local lookup and check for
		     * previous declarations of same name
		     */
		    
		   if (!dectest(&$3, symtab_cur))
			yyerror ("multiply defined identifier");
		   SET_BITS($3->type,c_pointer, c_set);
		   SET_BITS($3->type,e_var,e_set);
		   SET_BITS($3->type,$1,t_set);
		   $$=$3;
	      }
          ;
function_body : internal_decls statement_list  { $$=$2; }

internal_decls : declaration internal_decls  { $$=$1 }
               | {$$=(struct entry *) NULL;}
	       ;

statement_list : stat_mid statement_list { 
                    if (!($1))
			 yyerror ("null statement.  we don't like them.  sorry.");
		    $1->next = $2;
		    $$ = $1;
	       } 
				      /* if you really wanted to be */
				      /* perfect you would attach the */
				      /* root node of the statment tree */
				      /* after every statement; i don't */
				      /* think it's really necessary */
	       | { $$=(struct t_node *) NULL; }
		;
stat_mid : statement { $$ = $1; 
				      /* we are recording a global */
				      /* offset for temp vars which */
				      /* grows as each statement */
				      /* progresses and gets deleted at */
				      /* the end of a statement */
		       glob_tmp_offset = 0;
		     } 
	 ;  

statement : compound_stmt 
          | ';' { printf ("wow, a null stmt\n"); $$ = (struct t_node *) NULL; }
          | expression ';' 
          | if_stmt 
	  | for_stmt 
          | while_stmt
          | dowhile_stmt
          | return_stmt
          ;

compound_stmt : '{' jump_scope_norm internal_decls statement_list '}' 
                {
		     $$ = $4;
				/* we never want to free the elements of */
				/* the popped table; this was the only */
				/* one where we were previously doing that */
		     symtab_cur=pop_symtab(symtab_cur,0); 
		}
              ;

if_stmt : IF '(' expression ')' statement %prec LOWER_THAN_ELSE 
          {
	       $$=mknod(IF_N);
	       $$->EXPR = $3;
	       $$->STMT = $5;
	       $$->ELS = (struct t_node *) NULL;
	  }

        | IF '(' expression ')' statement ELSE statement
          {
	       $$=mknod(IF_N);
	       $$->EXPR = $3;
	       $$->STMT = $5;
	       $$->ELS = $7;
	  }
        ;
for_stmt : FOR '(' expression ';' expression ';' expression ')' statement 
           {
		$$=mknod(FOR_N);
		$$->INIT = $3;
		$$->EXPR = $5;
		$$->INCR = $7;
		$$->STMT = $9;
	   }
while_stmt : WHILE '(' expression ')' statement 
             {
		  $$=mknod(DO_N);
		  $$->EXPR = $3;
		  $$->STMT = $5;
		  $$->subtype = WHILE; /* a flag do differentiate DO/WHILE */
	     }
dowhile_stmt : DO statement WHILE '(' expression ')' ';'
               {
		    $$=mknod(DO_N);
		    $$->EXPR = $5;
		    $$->STMT = $2;
		    $$->subtype = DO; /* a flag do differentiate DO/WHILE */
	       }
return_stmt : RETURN expression ';' 
              {

				/* the messy type checking that we see */
				/* in tree.c is here, also */
		   
		   struct t_node *tmp_node;
		   HalfWord c1, c2, t1, t2;

		   c2 = ISO_BITS(glob_return_type,  c_set);
		   c1 = ISO_BITS($2->subtype,  c_set);
		   t2 = ISO_BITS(glob_return_type, t_set);
		   t1 = ISO_BITS($2->subtype,  t_set);

		   
		   if (c1 == c_array) /* this is a design flaw; array and */
				      /* pointer are equiv. here; make it so */
			c1 = c_pointer;
		   if (c2 == c_array)
			c2 = c_pointer;
		   
		   if (c1 != c2)
			yyerror("return type mismatch");
		   
		   else if (c1 == c_scalar)
		   {
			if (t1 != t2)
			{
			     if ((t1 == t_float) && (t2 == t_int))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;			      
				  $2->val.token = FTOI;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_int, t_set);
			     }
			     if ((t1 == t_float) && (t2 == t_char))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;			      
				  $2->val.token = FTOC;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_char, t_set);
			     }
			     if ((t1 == t_int) && (t2 == t_float))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;			      
				  $2->val.token = ITOF;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_float, t_set);
				  
			     }
			     if ((t1 == t_int) && (t2 == t_char))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;
				  $2->val.token = ITOC;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_char, t_set);
			     }
			     if ((t1 == t_char) && (t2 == t_float))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;
				  $2->val.token = CTOF;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_float, t_set);
			     }
			     if ((t1 == t_char) && (t2 == t_int))
			     {
				  /*
				   * make a new node just like
				   * $2; change $2
				   * to point to the new node
				   */
				  
				  tmp_node = mknod($2->type);
				  tmp_node->subtype = $2->subtype;
				  tmp_node->val = $2->val;
				  copy_children(tmp_node, $2);
				  $2->type = U_N;
				  $2->val.token = CTOI;
				  $2->CHILD = tmp_node;
				  SET_BITS($2->subtype, t_int, t_set);
				  
			     }
			}
		   }
		   
		   $$ = mknod(U_N);
		   $$->val.token = RETURN;
		   $$->CHILD = $2;
	      }
            | RETURN ';'
              { 

		   if (!(CHECK_BITS(t_void, glob_return_type, t_set)))
			yyerror("return type must match function");
		   $$ = mknod(U_N);
		   $$->val.token = RETURN;
		   $$->CHILD = NULL;
	      }

expression : unary_expr '=' expression 
	     {
		  $$=mknod(T_N);
		  $$->val.token = '=';
		  $$->LEFT=$1;
		  $$->RIGHT=$3;
		  $$->subtype=$3->subtype; 
		  $$->offset = add_tmp_offset($$->subtype,symtab_cur);
		  if (!(CHECK_BITS(e_var,$1->subtype, e_set)))
		       yyerror("left side of expression must be an lvalue");
	     }
           | binary_expr { $$ = $1; }
           ;		  
/* 
 * bison is brain dead when resolving SR conflicts
 * with assoc rules unless we write out each rule
 */

binary_expr : binary_expr OR binary_expr {
 
     if (((ISO_BITS($1->subtype, c_set)) != c_scalar) || 
	 ((ISO_BITS($3->subtype, c_set)) != c_scalar))
	  yyerror ("AND/OR arguments must be scalar");

     $$=mknod(T_N); $$->val.token = OR;$$->LEFT = $1;
     $$->RIGHT = $3; 
     $$->subtype = c_scalar|t_int|e_var;
     $$->offset = add_tmp_offset($$->subtype,symtab_cur);
}
            | binary_expr AND binary_expr { 
     if (((ISO_BITS($1->subtype, c_set)) != c_scalar) || 
	 ((ISO_BITS($3->subtype, c_set)) != c_scalar))
     {
	  yyerror ("AND/OR arguments must be scalar");
     }

     $$=mknod(T_N);$$->val.token = AND; $$->LEFT = $1;
     $$->RIGHT = $3; 
     $$->subtype = c_scalar|t_int|e_var;
     $$->offset = add_tmp_offset($$->subtype,symtab_cur);
}
            | binary_expr '+' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '+'))
		      yyerror("type conflict");
	    }
            | binary_expr '-' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '-'))
		      yyerror("type conflict");
	    }
            | binary_expr '*' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '*'))
		      yyerror("type conflict");
		 
				/* only set up call .mul on non floating pt */

		 if (!(CHECK_BITS(t_float,$$->subtype,t_set)))
		 {
		      /* potential problem: $$ gets an offset */
		      /* but we don't use it; we discard it  */
		      
		      tmp_node = mknod(U_N);
		      tmp_node->subtype = $$->subtype;
		      tmp_node->offset =  add_tmp_offset(tmp_node->subtype,
					  symtab_cur);
		      tmp_node->val.token = CALL;
		      tmp_node->CHILD = mknod(LEAF_N);
		      tmp_node->CHILD->subtype = $$->subtype;
		      tmp_node->CHILD->val.lf->args=$1;
		      $1->next = $3;
		      tmp_node->CHILD->val.lf->num.sym = 
			   creat_symbol(".mul", 0);
		      $$ = tmp_node; /* memory leak on */
				     /* prev $$ ... */
		 }
	    }
            | binary_expr '/' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '/'))
		      yyerror("type conflict");
		 
				/* only set up call .mul on non floating pt */

		 if (!(CHECK_BITS(t_float,$$->subtype,t_set)))
		 {
		      /* potential problem: $$ gets an offset */
		      /* but we don't use it; we discard it  */
		      
		      tmp_node = mknod(U_N);
		      tmp_node->subtype = $$->subtype;
		      tmp_node->offset = 
			   add_tmp_offset(tmp_node->subtype,symtab_cur);
		      tmp_node->val.token = CALL;
		      tmp_node->CHILD = mknod(LEAF_N);
		      tmp_node->CHILD->subtype = $$->subtype;
		      tmp_node->CHILD->val.lf->args=$1;
		      $1->next = $3;
		      tmp_node->CHILD->val.lf->num.sym = 
			   creat_symbol(".div", 0);
		      $$ = tmp_node; /* memory leak on */
				     /* prev $$ ... */
		 }
	    }
            | binary_expr EQ binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, EQ))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);

	    }
            | binary_expr NEQ binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, NEQ))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);
	    }
            | binary_expr GEQ binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, GEQ))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);
	    }
            | binary_expr '<' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '<'))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);
	    }
            | binary_expr '>' binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, '>'))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);
	    }
            | binary_expr LEQ binary_expr  {
		 if (!typecheck_binary (&$$, $1, $3, LEQ))
		      yyerror("type conflict");
		 SET_BITS($$->subtype, c_scalar, c_set);
	    }
            | unary_expr {}
            ;

unary_expr: unary_op unary_expr
            {
		 $$=mknod(U_N);
		 $$->val.token = $1;
		 $$->CHILD=$2;
		 if ($1 == '!')
		 {
		      SET_BITS($$->subtype,t_int,t_set);
		      SET_BITS($$->subtype,c_scalar,c_set);
				/* not concerned about e_type for tnode */
		 }
		 if (($1 == '+') || ($1 == '-') || ($1 == PLUS) ||
		     ($1 == MINUS))
		 {
		      if ((ISO_BITS($2->subtype, c_set)) != c_scalar)
			 yyerror ("+/-/++/--: operands must be scalar lvalue");

		      $$->subtype = $1;
		      SET_BITS($$->subtype,t_int,t_set);
		      SET_BITS($$->subtype,c_scalar,c_set);
				/* not concerned about e_type for tnode */
		 }
		 if ($1 == ADDROF)
		 {
		      if (((ISO_BITS($2->subtype, c_set)) == c_pointer) ||
			  ((ISO_BITS($2->subtype, c_set)) == c_array))
			   yyerror ("&: cannot address-of a pointer");
		      $$->subtype = $2->subtype; /* same type but a pointer */
		      SET_BITS($$->subtype,c_pointer,c_set);
		      $$->offset = add_tmp_offset($$->subtype,symtab_cur);
		 }
		 if ($1 == DEREF)
		 {

		      if (((ISO_BITS($2->subtype, c_set)) != c_pointer) &&
			  ((ISO_BITS($2->subtype, c_set)) != c_array))
			   yyerror ("*: cannot dereference a non-pointer");
		      $$->subtype = $2->subtype; /* same type but a scalar */ 
		      SET_BITS($$->subtype,c_scalar,c_set);
		      $$->offset = add_tmp_offset($$->subtype,symtab_cur);
		 }
	    }
	    | postfix_expr_root
 
unary_op : '!' {$$='!';} | '+'{$$='+';} | '-' {$$='-';}| PLUS {$$=PLUS;}
         | MINUS {$$=MINUS;}| '&' {$$=ADDROF;}| '*'{$$=DEREF;} ;

postfix_expr_root : postfix_expr_root PLUS
	     {
		  $$=mknod(U_N);
		  $$->val.token = PLUS;
		  $$->CHILD=$1;
	     }
		  
             | postfix_expr_root MINUS
	     {
		  $$=mknod(U_N);
		  $$->val.token = MINUS;
		  $$->CHILD=$1;
	     }
             | postfix_expr
             ;

postfix_expr : postfix_expr '[' expression ']' /* array index calc */
	       {
		    
		    tmp_node = mknod(T_N);
		    tmp_node->val.token = ARYNDX;
		    tmp_node->LEFT = $1;
		    tmp_node->RIGHT = $3;
		    tmp_node->subtype = $1->subtype;
					       /* set subtype to that of */
					       /* the array */
		    
		    tmp_node->offset = 
			 add_tmp_offset(tmp_node->subtype,symtab_cur);

		    SET_BITS(tmp_node->subtype,c_pointer,c_set);

		    $$ = mknod(U_N);
		    $$->val.token = DEREF;
		    $$->CHILD = tmp_node;
		    $$->subtype = tmp_node->subtype;


		    SET_BITS($$->subtype,c_scalar,c_set);
		    $$->offset = add_tmp_offset($$->subtype,symtab_cur);

	       }
             | ID '(' argument_list ')'
               {

		    if ($1->references < 1) /* should have at least one ref */
			 yyerror ("undefined identifier");

		    tmp_node=mknod(LEAF_N);
		    tmp_node->subtype=$1->type;
		    tmp_node->val.lf->num.sym = $1;
		    tmp_node->val.lf->args=$3; /* set subtype */

				/* check parameters' types with proto */
		    if (!check_params($3, $1->next_fn))
			 yyerror("parameters in fn call don't match prototype");

		    $$ = mknod(U_N);
		    $$->val.token = CALL;
		    $$->CHILD = tmp_node;
					 /* same type as calling fn, but */
					 /* make it a var, not an fn so */
					 /* that it will get an offset space */
		    $$->subtype = $1->type;
		    SET_BITS($$->subtype,e_var,e_set);
		    $$->offset = add_tmp_offset($$->subtype,symtab_cur);

	       }
             | primary_expr { $$ = $1; }
             ;
primary_expr : ID
               { 

		    if ($1->references < 1) /* should have at least one ref */
			 yyerror ("undefined identifier");

		    $$=mknod(LEAF_N);
		    $$->subtype=$1->type;
		    $$->val.lf->num.sym = $1;
	       }
             | constant { $$ = $1; }
	     | '(' expression ')' { $$ = $2; }
             ;

constant : ICON {
     $$=mknod(LEAF_N);
     $$->val.lf->num.sym=$1;
     $$->subtype = $1->type;
     add_const(&glob_const_list, $1);
}
         | CCON {
     $$=mknod(LEAF_N);
     $$->val.lf->num.sym=$1;
     $$->subtype = $1->type;
     add_const(&glob_const_list, $1);
}

         | SCON {
     $$=mknod(LEAF_N);
     $$->val.lf->num.sym=$1;
     $$->subtype = $1->type;
     add_const(&glob_const_list, $1);
}

         | FCON {
     $$=mknod(LEAF_N);
     $$->val.lf->num.sym=$1;
     $$->subtype = $1->type;
     add_const(&glob_const_list, $1);
}
         ;

argument_list : expression { $$ = $1; $$->next = NULL; }
              | argument_list ',' expression
                { 
		     tmp_node=$1;
		     while ($1)
		     {
			  if ($1->next)
			       $1=$1->next;
			  else
			       break;
		     }
		     $1->next=$3;
		     $$=tmp_node;
		}
              | { $$ = (struct t_node *) NULL; }
              ;
%%

yyerror(const char *msg)
{
     printf ("error at line %d: %s \n",yylineno, msg);
     abort();
}


void yywarning(const char *msg)
{
     printf ("warning at line %d: %s \n",yylineno, msg);
}

main(int argc, char **argv)
{ 

     /*
      * initialize globals, grab command line arg, 
      * open input file, parse it, determine output
      * file name, open output file, generate code 
      */

     extern FILE *yyin;
     char filename[256];
     char *ptr;

     glob_parm_offset = 68;		       /* [%fp+68] */
     sym_debug=0;
     lookups=0;
     linear_jumps=0;
     glob_offset=0;
     tree_root = NULL;
     glob_const_offset=0;
     glob_const_list = NULL;

     symtab_l = creat_symtab(symtab_l);
     symtab_cur = symtab_l;

     if (argc != 2)			       /* check cmd line args */
     {
	  printf ("usage: %s filename.c\n",
		  argv[0]);
	  exit ( 2 );
     }

     strcpy (filename, argv[1]);	       /* grab fname from cmd line */

     yyin = fopen (filename, "r");	       /* can we read it? */
     if (!yyin)
	  yyerror ("unable to open file");

     yyparse();				       /* lexer -> parser -> trees */

     ptr = strchr (filename, '.');	       /* .c -> .s */
     if (!ptr)
	  yyerror ("please include a .c in your filename");
     *ptr = 0;
     strcat (filename, ".s");

     out=fopen(filename,"w");		       /* open output file */
     if (!out)
     {
	  perror("unable to open output file");
	  exit( 2 );
     }

     codegen(tree_root);		       /* generate code */
     fclose(out);			       /* done */
}

