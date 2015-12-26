/*
 * register.c
 *
 * $Id: register.c,v 1.2 1996/09/17 23:50:59 berrym Exp $ 
 * $Log: register.c,v $
 * Revision 1.2  1996/09/17  23:50:59  berrym
 * made the debugging stuff look nicer
 *
 * Revision 1.1  1996/09/17  21:15:20  berrym
 * Initial revision
 * 
 *
 * Register allocator and code generation routines
 *
 * Mike Berry
 * University of Pennsylvania
 */

#include "globals.h"
#include "mcc.tab.h"

int find_offset (struct t_node *node)
{

     /*
      * return an offset, either from within the node itself,
      * or from the node's member symbol
      */

     if (!node)
	  return 0;

     if (node->type == LEAF_N)
	  return (node->val.lf->num.sym->offset);
     else
	  return (node->offset);
}

void codegen(struct t_node *root)
{
     /*
      * generate assembly code given a syntax tree list
      */

     struct reg *regs;
     
     reginit (&regs);		/* initialize the register list */

     labelno = 0;

     fprintf(out,"\t.data\n");
     code_const(glob_const_list);
     fprintf(out,"\t.align 4\n");
     fprintf(out,"\t.global %s\n",glob_mainfn->name);
     fprintf(out,"%s:\n",glob_mainfn->name);
     fprintf(out,"\tsave\t%csp,%d,%csp\n",0x25,(-64 - 4 - 24 + glob_offset) & -8,0x25); 

     pull_in_registers(glob_mainfn->next_fn);
     code_statements(root,regs);
     flushall(regs,-1);		/* flush all registers -- we're done */
     fprintf(out,".LLR:\n",labelno++);
     fprintf(out,"\tret\n");
     fprintf(out,"\trestore\n");
}

void code_const (struct entry *list)
{

     /*
      * for simplicity, we're putting all constants, including integers,
      * into the .data section -- this function does that
      */

     while (list)
     {

	  if ((CHECK_BITS(t_float,list->type,t_set)))
	  {
	       fprintf (out,"\t.align 4\n");
	       fprintf (out,".LLC%d:",
			list->offset);
	       fprintf (out,"\t.single\t0r%s\n",
			list->name);
	  }
	  else if ((CHECK_BITS(t_int,list->type,t_set)))
	  {
	       fprintf (out,"\t.align 4\n");
	       fprintf (out,".LLC%d:",
			list->offset);
	       fprintf (out,"\t.word\t%d\n",
			atoi(list->name));

	  }
	  else if ((CHECK_BITS(t_char,list->type,t_set)))
	  {
	       if ((CHECK_BITS(c_scalar,list->type,c_set)))
	       {
		    fprintf (out,"\t.align 2\n");
		    fprintf (out,".LLC%d:",
			     list->offset);
		    fprintf (out,"\t.byte\t\"%c\"\n",
			     *(list->name+1));
	       }
	       else		/* pointer (string) */
	       {
		    fprintf (out,"\t.align 4\n");
		    fprintf (out,".LLC%d:",
			     list->offset);
		    fprintf (out,"\t.asciz\t%s\n",
			     list->name);


	       }
	  }

	  list = list->next_const;
     }



}

void code_statements(struct t_node *tree, struct reg *regs)
{

     /*
      * generate code for a list of stmts
      */

     while (tree)
     {
	  code_statement(tree, regs);
	  tree = tree->next;
     }

}
	       

void code_statement(struct t_node *tree, struct reg *regs)
{

     /*
      * recursively generate code for a single stmt and its children
      *
      * probably the longest single function in the compiler -- breaking it
      * up would be nice..
      */

     int offset_left;
     int offset_right;
     int labelsave;
     struct reg *reg1;		/* tmp register holders */
     struct reg *reg2;
     struct reg regi;
     char regchar;
     char cmd1[16];
     char cmd2[16];
	

     regi.type = 'i';		   /* just a dummy for %i0 */
     regi.num = 0;

     if (tree->type == T_N)
     {
	  if ((tree->LEFT->type == T_N) ||
	      (tree->LEFT->type == U_N))
	  {
	       code_statement(tree->LEFT, regs);
	       code_statement(tree->RIGHT, regs);
	  }
	  else
	  {
	       code_statement(tree->LEFT, regs);
	       code_statement(tree->RIGHT, regs);
	  }
	  
	  /* get offsets for children */

	  offset_left = find_offset(tree->LEFT);
	  offset_right = find_offset(tree->RIGHT);

	  switch (tree->val.token)
	  {
	  case ARYNDX:

	       debug("aryndx","start, alloc for left,right",tree);
	       while (1) 
	       { 
		    reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1); 
							/*multiplicand */
		    if (CHECK_BITS(c_pointer,tree->LEFT->subtype,c_set))
			 reg2 = regalloc(regs, tree->LEFT, 0, 'o',1,-1);
		    else
			 reg2 = regalloc(regs, tree->LEFT, 0, 'o',0,-1);
							  /* just */
							  /*using for tmpval*/
		    if ((reg1)  && (reg2))
			 break; 
		    flushall(regs, 'o'); 
	       } 

				/* notice: this addrof calculation can */
				/* always be done at compile time; */
				/* doesn't need to be calculated at */
				/* runtime -- that's why we have the */
				/* following restriction: */

	       if (tree->LEFT->type != LEAF_N)
		    yyerror ("invalid lvalue in unary &");
	       
	       flush(reg1);
	       reg2->node = NULL; /* minimal flush */
	       reg2->dontflush = 0;	       

	       /* add  %fp,reg2,reg2 */

	       if (!(CHECK_BITS(c_pointer,tree->LEFT->subtype,c_set)))
		    fprintf (out,"\tadd\t%cfp,%d,%c%c%d\n",
			     0x25,find_offset(tree->LEFT),0x25,reg2->type,reg2->num);

	       /*
		* make this value the product of ->RIGHT and 4 or 2 (multipliers)
		* subtracted from the offset of ->LEFT
		*/

	       if (CHECK_BITS(t_char,tree->subtype,t_set))
		    fprintf (out,"\tsll\t%c%c%d,1,%c%c%d\n",
			     0x25,reg1->type,reg1->num,0x25,reg1->type,reg1->num);
	       else
		    fprintf (out,"\tsll\t%c%c%d,2,%c%c%d\n",
			     0x25,reg1->type,reg1->num,0x25,reg1->type,reg1->num);

	       emit("add",reg2,reg1,reg1);
	       
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */
	       



	       break;

	  case '+':
	  case '-':

	       if (CHECK_BITS(t_float,tree->subtype,t_set))
	       {
		    regchar = 'f';
		    strcpy(cmd1,"fadds");
		    strcpy(cmd2,"fsubs");
	       }
	       else
	       {
		    regchar = 'o';
		    strcpy(cmd1,"add");
		    strcpy(cmd2,"sub");
	       }


	       while (1) 
	       { 
		    reg1 = regalloc(regs, tree->LEFT, 0, regchar,1,-1);
		    reg2 = regalloc(regs, tree->RIGHT, 0, regchar,1,-1); 
		    if ((reg1) && (reg2)) break; 
		    flushall(regs, regchar); 
	       } 
	       flush(reg1); 
	       if (tree->val.token == '+') 
		    emit (cmd1,reg1,reg2,reg1); 
	       else if (tree->val.token == '-') 
		    emit (cmd2,reg1,reg2,reg1);
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */
	       break;
	  case '*':
	  case '/':

				/* hit this only on floating pt */

	       if (CHECK_BITS(t_float,tree->subtype,t_set))
	       {
		    regchar = 'f';
		    strcpy(cmd1,"fmuls");
		    strcpy(cmd2,"fdivs");
	       }
	       else
	       {
		    yyerror("internal error: tried to / or * a non fp");
	       }


	       while (1) 
	       { 
		    reg1 = regalloc(regs, tree->RIGHT, 0, regchar,1,-1);
		    reg2 = regalloc(regs, tree->LEFT, 0, regchar,1,-1); 
		    if ((reg1) && (reg2)) break; 
		    flushall(regs, regchar); 
	       } 
	       flush(reg1); 
	       if (tree->val.token == '*') 
		    emit (cmd1,reg1,reg2,reg1); 
	       else if (tree->val.token == '/') 
		    emit (cmd2,reg1,reg2,reg1);
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */


	       break;
	  case OR:

	       while (1)
	       {
		    reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
		    reg2 = regalloc(regs, tree->LEFT, 0, 'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }
	       flush(reg1);
	       emit ("or",reg1,reg2,reg1);
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */

	       break;
	  case AND:

	       while (1)
	       {
		    reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
		    reg2 = regalloc(regs, tree->LEFT, 0, 'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }
	       flush(reg1);
	       emit ("and",reg1,reg2,reg1);
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */

	       break;
	  case NEQ:
	  case EQ:

	       while (1)
	       {
		    reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
		    reg2 = regalloc(regs, tree->LEFT, 0, 'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }
	       flush(reg2);
	       flush(reg1);
	       emit ("xor",reg1,reg2,reg2);
	       emit ("subcc",g0_reg,reg2,g0_reg);
	       if (tree->val.token == EQ)
		    fprintf (out,"\tsubx\t%cg0,-1,%c%c%d\n",	/* blah */
			     0x25,0x25,reg1->type,reg1->num);
	       else if (tree->val.token == NEQ)
		    fprintf (out,"\taddx\t%cg0,-1,%c%c%d\n",	/* blah */
			     0x25,0x25,reg1->type,reg1->num);
	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */

	       break;

	  case GEQ:
	  case LEQ:
	  case '<':
	  case '>':

	       debug("compare","flush then alloc regs for left,right",tree);
	       flushall(regs,'o');
		    reg1 = regalloc(regs, tree->LEFT, 0, 'o',1,-1); 
		    reg2 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
		    if ((!reg1) || (!reg2))
			 yyerror("couldn't allocate enough registers");

	       emit2("cmp",reg1,reg2);	

	       debug("compare","branch next",tree);
	       if (tree->val.token == GEQ)	       
		    fprintf(out,"\tbge,a\t.LL%d\n",labelno);
	       else if (tree->val.token == LEQ)	       
		    fprintf(out,"\tble,a\t.LL%d\n",labelno);
	       else if (tree->val.token == '>')	       
		    fprintf(out,"\tbg,a\t.LL%d\n",labelno);
	       else if (tree->val.token == '<')	       
		    fprintf(out,"\tbl,a\t.LL%d\n",labelno);
	       fprintf(out,"\tmov\t1,%c%c%d\n",
		       0x25,reg1->type,reg1->num);
	       fprintf(out,"\tmov\t0,%c%c%d\n",
		       0x25,reg1->type,reg1->num);
	       fprintf(out,".LL%d:\n",labelno++);


	       reg1->node = tree;
	       reg1->dontflush = 0; /* whenever you reassign a register, */
				    /* you need to reassign the node value */
				    /* and the dontflush value!! */

	       break;

	  case '=':
	       if (tree->LEFT->type == LEAF_N)
	       {
		    while (1)
		    {
			 reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
			 /* do not refresh lval */
			 reg2 = regalloc(regs, tree->LEFT, 0, 'o',0,-1);
			 if ((reg1) && (reg2))
			      break;
			 flushall(regs,'o');
		    }
		    
		    emit2 ("mov", reg1, reg2);
		    flush(reg2);
	       }
	       else		/* something like x[5] */
	       {
		    if (tree->LEFT->val.token != DEREF)
			 yyerror ("Unknown lval type on the left side of =");
		    while (1)
		    {
			 reg1 = regalloc(regs, tree->RIGHT, 0, 'o',1,-1);
			 reg2 = regalloc(regs, tree->LEFT->CHILD, 0, 'o',1,-1);
			 if ((reg1) && (reg2))
			      break;
			 flushall(regs,'o');
		    }
		    
		    fprintf (out,"\tst\t%c%c%d,[%c%c%d]\n",
			     0x25,reg1->type,reg1->num,0x25,reg2->type,reg2->num);


	       }
	       
	       break;
	  };

     }
     else if (tree->type == IF_N)
     {
	  labelsave = labelno;
	  labelno = labelno + 3;

	  debug("if","expression begins here",tree);
	  code_statement(tree->EXPR, regs);
	  debug("if","expression done.  flush regs",tree);
	  flushall(regs, 'o');
	  debug("if","alloc a single reg for expr",tree);
	  reg1 = regalloc(regs, tree->EXPR, 0, 'o',1,-1);
	  if (!reg1)
	       yyerror("couldn't allocate register");
	  debug("if","got reg.  now subcc for if",tree);

	  fprintf(out,"\tsubcc\t%c%c%d,%cg0,%cg0\n",
		  0x25,reg1->type,reg1->num,0x25,0x25);

	  reg1->node = NULL;
	  reg1->dontflush = 0;

	  fprintf(out,"\tbne\t.LL%d\n",
		  labelsave);
	  fprintf(out,"\tnop\n");

	  fprintf(out,"\tba\t.LL%d\n",
		  labelsave+1);
	  fprintf(out,"\tnop\n");

	  debug("if","flush all regs in prep for stmts",tree);

	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */
	  fprintf(out,".LL%d:\n",
		  labelsave);

	  debug("if","stmt next",tree);
	  code_statements(tree->STMT, regs);
	  debug("if","stmt done.  flush regs.",tree);

	  flushall(regs,'o');
	  fprintf(out,"\tba\t.LL%d\n",
		  labelsave+2);
	  fprintf(out,"\tnop\n");

	  debug("if","els next.  flush.",tree);

	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */

	  fprintf(out,".LL%d:\n",
		  labelsave+1);

	  code_statements(tree->ELS, regs);
	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */

	  fprintf(out,".LL%d:\n",
		  labelsave+2);
	  debug("if","els done.  if done.",tree);

     }
     else if (tree->type == FOR_N)
     {
	  debug("for","start",tree);
	  labelsave = labelno;
	  labelno = labelno + 2;
	  code_statement(tree->INIT, regs);
	  debug("for","finished init.  flush regs.",tree);

	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */
	  debug("for","expr next.",tree);

	  fprintf(out,".LL%d:\n",
		  labelsave);


	  code_statement(tree->EXPR, regs);
	  debug("for","expr done. flush",tree);

	  flushall(regs,'o');

	  reg1 = regalloc(regs, tree->EXPR, 0, 'o',1,-1);
	  if (!reg1)
	       yyerror("unable to alloc register");

	  debug("for","expr test next",tree);
	  fprintf(out,"\tsubcc\t%c%c%d,%cg0,%cg0\n",
		  0x25,reg1->type,reg1->num,0x25,0x25);
	  reg1->node = NULL;
	  reg1->dontflush = 0;

	  fprintf(out,"\tbe\t.LL%d\n",
		  labelsave+1);
	  fprintf(out,"\tnop\n");


	  debug("for","stmt,incr next",tree);

	  code_statements(tree->STMT, regs); /* stmt may be MULTIPLE!!! */
	  code_statement(tree->INCR, regs);

	  debug("for","stmt,incr done. flush",tree);
	  flushall(regs,'o');

	  fprintf(out,"\tba\t.LL%d\n",
		  labelsave);
	  fprintf(out,"\tnop\n");

	  debug("for","no mans land. flush",tree);
	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */
	  fprintf(out,".LL%d:\n",
		  labelsave+1);



     }
     else if (tree->type == DO_N)
     {
	  if (tree->subtype == WHILE)
	  {
	       labelsave = labelno;
	       labelno = labelno + 3;
	       flushall(regs, 'o'); /* flush all registers after */
				    /* every label */
	       fprintf(out,".LL%d:\n",
		       labelsave);


	       code_statement(tree->EXPR, regs);

	       flushall(regs, 'o');
	       reg1 = regalloc(regs, tree->EXPR, 0, 'o',1,-1);
	       if (!reg1)
		    yyerror("unable to alloc register");

	       fprintf(out,"\tsubcc\t%c%c%d,%cg0,%cg0\n",
		       0x25,reg1->type,reg1->num,0x25,0x25);
	       reg1->node = NULL;
	       reg1->dontflush = 0;
	       
	       
	       fprintf(out,"\tbe\t.LL%d\n",
		       labelsave+2);
	       fprintf(out,"\tnop\n");
	       
	       flushall(regs, 'o'); /* flush all registers after */
				    /* every label */
	       fprintf(out,".LL%d:\n",
		       labelsave+1);
	       
	       
	       code_statements(tree->STMT, regs);

	       flushall(regs, 'o');
	       fprintf(out,"\tba\t.LL%d\n",
		       labelsave);
	       fprintf(out,"\tnop\n");
	       flushall(regs, 'o'); /* flush all registers after */
				    /* every label */
	       fprintf(out,".LL%d:\n",
		       labelsave+2);




	  }
	  else			/* it's a do-while */
	  {
	       labelsave = labelno;
	       labelno++;
	  flushall(regs, 'o'); /* flush all registers after */
			       /* every label */
	       fprintf(out,".LL%d:\n",
		       labelsave);


	       code_statements(tree->STMT, regs);
	       code_statement(tree->EXPR, regs);

	       flushall(regs,'o');
	       reg1 = regalloc(regs, tree->EXPR, 0, 'o',1,-1);
	       if (!reg1)
		    yyerror("unable to alloc register");
	       
	       fprintf(out,"\tsubcc\t%c%c%d,%cg0,%cg0\n",
		       0x25,reg1->type,reg1->num,0x25,0x25);
	       
	       reg1->node = NULL;
	       reg1->dontflush = 0;


	       fprintf(out,"\tbne\t.LL%d\n",
		       labelsave);
	       fprintf(out,"\tnop\n");
	  }

     }

     else if (tree->type == U_N)
     {
	  if (tree->val.token == DEREF)
	  {
	       debug("deref","coding the child",tree);
	       code_statement(tree->CHILD, regs);
	       debug("deref","coding the child done",tree);
	       while (1)
	       {		/* might be able to use just one */
				/* register for this op */
		    reg1 = regalloc(regs, tree->CHILD, 0, 'o',1,-1);
		    reg2 = regalloc(regs, tree, 0, 'o',0,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }
	       debug("deref","now loading child/me regs",tree);

	       fprintf (out,"\tld\t[%c%c%d],%c%c%d\n",
			0x25,reg1->type,reg1->num,0x25,reg2->type,reg2->num);

	  }
	  else if ((tree->val.token == PLUS) ||
		   (tree->val.token == MINUS))
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs,tree->CHILD,0,'o',1,-1);
		    if (reg1)
			 break;
		    flushall(regs, 'o');
	       }

	       if (tree->val.token == PLUS) 
		    fprintf (out,"\tinc\t%c%c%d\n",
			     0x25,reg1->type,reg1->num);
	       else
		    fprintf (out,"\tdec\t%c%c%d\n",
			     0x25,reg1->type,reg1->num);

	  }



	  else if (tree->val.token == ITOF)
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'f',0,-1);
		    reg2 = regalloc(regs,tree->CHILD,0,'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }

	       emit2("fitos",reg1,reg2);
	       

	  }

	  else if ((tree->val.token == ITOP) ||
		   (tree->val.token == CTOP))
	  {
	       int sllnum;

	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'f',0,-1);
		    reg2 = regalloc(regs,tree->CHILD,0,'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }

	       if (CHECK_BITS(t_char,tree->subtype,t_set))
		    sllnum = 1;
	       else
		    sllnum = 2;


	       fprintf (out,"\tsll\t%c%c%d,sllnum,%c%c%d\n",
			0x25,reg2->type,reg2->num,sllnum,
			0x25,reg1->type,reg1->num);
			
	       

	  }
	  else if (tree->val.token == FTOI)
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'f',1,-1);
		    reg2 = regalloc(regs,tree->CHILD,0,'o',0,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }

	       emit2("fstoi",reg1,reg2);
	       

	  }
	  else if (tree->val.token == FTOC)
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'f',1,-1);
		    reg2 = regalloc(regs,tree->CHILD,0,'o',0,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }

	       emit2("fstoi",reg1,reg2);
	       

	  }

	  else if (tree->val.token == CTOF)
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'f',0,-1);
		    reg2 = regalloc(regs,tree->CHILD,0,'o',1,-1);
		    if ((reg1) && (reg2))
			 break;
		    flushall(regs, 'o');
	       }

	       emit2("fitos",reg1,reg2);
	       

	  }
	  else if (tree->val.token == ADDROF)
	  {
	       code_statement(tree->CHILD, regs);
	       while (1)
	       {
		    reg1 = regalloc(regs, tree, 0, 'o',0,-1);
		    if (reg1)
			 break;
		    flushall(regs, 'o');
	       }	       

				/* notice: this addrof calculation can */
				/* always be done at compile time; */
				/* doesn't need to be calculated at */
				/* runtime -- that's why we have the */
				/* following restriction: */

	       if (tree->CHILD->type != LEAF_N)
		    yyerror ("invalid lvalue in unary &");

	       fprintf (out,"\tadd\t%cfp,%d,%c%c%d\n",
			0x25,find_offset(tree->CHILD),0x25,reg1->type,reg1->num);

	  }

	  else if (tree->val.token == RETURN)
	  {

	       if (tree->CHILD)
	       {
		    code_statement(tree->CHILD, regs);
		    /* 	  flushall(regs, 'o'); */
		    
		    while (1)
		    {
			 reg1 = regalloc(regs, tree->CHILD, 0, 'o',1,-1);
			 if (reg1)
			      break;
			 flushall(regs, 'o');
		    }
		    
		    emit2("mov",reg1, &regi);
	       }

	       fprintf (out,"\tba\t.LLR\n");
	       fprintf (out,"\tnop\n");
	       

				/* if there is no child, then just return */
	  }
	  else if (tree->val.token == CALL)
	  {
	       code_call(0,tree->CHILD->val.lf->args, regs);
	       flushall(regs,'o'); /* really we only need to flush o0 */
	       fprintf (out,"\tcall\t%s\n",
			tree->CHILD->val.lf->num.sym->name);
	       fprintf (out,"\tnop\n");

				   /* now just get the o0 into its stor */
				   /* on stack */
	       reg1 = regalloc(regs, tree, 0, 'o',0,-1);	  	
	       flushall(regs,'o');
	  }
     }
     else if (tree->type == LEAF_N)
     {
	  /* fprintf (out,"\tCODE FOR LEAF\n");*/
     }
     
}

void code_call (int arg_number, struct t_node *parameter_list, struct reg
	      *regs)
{

     
	   /* a recursive function to traverse a parameter forwards, */
	   /* then backwards */


     int x;
     struct reg *reg1;

     code_statement(parameter_list, regs);
     if (parameter_list->next)
	  code_call (arg_number+1, parameter_list->next, regs);
     else
     {
	  if (arg_number < 6)
	  {
	       flushall(regs,'o');
	  }
     }

     if (arg_number < 6)
     {
	  reg1 = regalloc(regs, parameter_list, 2, 'o',1,arg_number);	  	
	  if (!reg1)
	       yyerror("internal error.  unabled to allocate register for call.");
     }
     else			   /* above arg5 */
     {
	  while (1)
	  {
	       reg1 = regalloc(regs, parameter_list, 0, 'o',1,-1);	  
	       if (reg1)
		    break;
	       flushall(regs,'o');
	  }

	  fprintf (out,"\tst\t%c%c%d,%s\n",
		   0x25,reg1->type,reg1->num,fp(68+(4*arg_number)));

	  if (arg_number == 6)
	       flushall(regs,'o');
     }

}

void pull_in_registers (struct entry *fn)
{
     int reg = 0;
     while ((fn) && (reg < 6))
     {
	  fprintf (out,"\tst\t%ci%d,%s\n",
		   0x25,reg++,fp(fn->offset));

	  fn = fn->next_fn;
     }

}

char *fp (int offset)
{
     char chr;

     if (offset >= 0)
	  chr = '+';
     else
	  chr = ' ';

     sprintf (glob_str, "[%cfp%c%d]",
	      0x25,chr,offset);
     

     return glob_str;
}

void reginit (struct reg **regs)
{

     /*
      * initialize the register storage list
      */

     int x, y;
     struct reg *tmpreg;
     struct reg *savereg;
     for (x = 0; x < 5; x++)
     {
	  for (y = 0; y < 8; y++)
	  {
	       tmpreg = (struct reg *) malloc (sizeof (struct reg));
	       if ((!x) && (!y))
	       {
		    *regs = tmpreg;
	       }
	       else
	       {
		    savereg->next = tmpreg;
	       }
	       savereg = tmpreg;
	       tmpreg->node = NULL;
	       tmpreg->num = y;
	       tmpreg->dontflush = 0;
	       switch (x) {
	       case 0:
		    tmpreg->type = 'g';
		    if (y == 0)
		    {
			 tmpreg->reserved = 1;
			 g0_reg = tmpreg;
		    }
		    break;
	       case 1:
		    tmpreg->type = 'o';
		    if ((y == 6) || (y == 7))
			 tmpreg->reserved = 1; /* reserve %o6,%o7 */
		    else
			 tmpreg->reserved = 0;
		    break;
	       case 2:
		    tmpreg->type = 'l';
		    break;
	       case 3:
		    tmpreg->type = 'i';
		    if ((y == 6) || (y == 7))
			 tmpreg->reserved = 1; /* reserve %o6,%o7 */
		    else
			 tmpreg->reserved = 0;
		    break;
	       case 4:
		    tmpreg->type = 'f';
		    tmpreg->reserved = 0;
		    break;
	       };
	  }
     }
}


struct reg * search (struct reg *regs, struct t_node *node)
{

     /*
      * look to see if a node is already in a register
      */

     while (regs)
     {
	  if ((regs->node) && (node))
	  {
				/* i don't think this will work if the */
				/* offset is calculated at runtime */
	       /*	       if (find_offset(regs->node) == find_offset(node))
			       return regs;*/

	       if (regs->node == node)
		    return regs;


	  }
	  regs = regs->next;
     }
     
     return NULL;
}

struct reg * regalloc (struct reg *regs, struct t_node *node, 
		       int requirements,char type, int refresh_flag, int regnum)
{

     /*
      * the register allocator
      * regnum is -1 for any register, 0-6 for requested register
      */

     /* right now, requirements is 1 if we don't want to look for an */
     /* existing register for this tree value -- 2 is same...*/

     int offset;
     struct reg *tmpreg;
     struct reg *lastreg;
     offset = find_offset(node);

     tmpreg = search(regs,node);
     if ((tmpreg) && (!requirements))
     {
	  if (((tmpreg->type == 'f') && (type != 'f')) ||
	      ((type == 'f') && (tmpreg->type != 'f')))
	  {
				/* save the register back into memory, then */
				/* load it back in as the new register type */
	       flush(tmpreg);
	       return (regalloc(regs,node,requirements,type,refresh_flag,regnum));
	  }
	  else
	       return tmpreg;
     }
     else
     {

	  while (regs)
	  {
	       if ((regs->type == type)   && (!(regs->node)) && (!(regs->reserved)))
	       {
		    if ((regnum < 0) || (regnum == regs->num))
		    {
			 regs->node = node;
			 if (refresh_flag)
			      refresh(regs);
			 return regs;
		    }
	       }
	       
	       regs = regs->next;
	  }
	  
     }
     return NULL;
}

void refresh (struct reg *regs)
{

     /*
      * reload into a register what is supposed to be there 
      */

     int offset;
     if (regs->node)
     {
	  offset = find_offset(regs->node);
	  if ((CHECK_BITS(e_const,regs->node->subtype,e_set)))
	  {
	       regs->dontflush = 1;

	       if ((CHECK_BITS(t_float,regs->node->subtype,t_set)))
	       {
		    /*
		     * we need to allocate a register to hold the address
		     * of the const; but we can't call regalloc because we
		     * might flush the register we're trying to refresh in
		     * the first place.  so we're going to use local
		     * registers for this purpose ONLY
		     */

		    fprintf (out,"\tset\t.LLC%d,%c%c%d\n",
			     offset,0x25,'l',0); /* l0 -- cheat */
		    fprintf (out,"\tld\t[%c%c%d],%c%c%d\n", /* ldf is */
							    /* assembled */
							    /* from ld */
			     0x25,'l',0,
			     0x25,regs->type,regs->num);
	       }
	       else
	       {
		    fprintf (out,"\tset\t.LLC%d,%c%c%d\n",
			     offset,0x25,regs->type,regs->num);
		    fprintf (out,"\tld\t[%c%c%d],%c%c%d\n",
			     0x25,regs->type,regs->num,
			     0x25,regs->type,regs->num);
	       }

	  }
	  else
	  {

	       fprintf (out,"\tld\t%s,%c%c%d\n",
			fp(offset),0x25,regs->type,regs->num);
	  }
     }

}

int amountused (struct reg *regs, char type)
{

     /*
      * how many registers of a given
      * type are used 
      */

     int x = 0;
     while (regs)
     {
	  if ((regs->type == type) && (regs->node))
	       x++;
	  regs = regs->next;
     }
     
     return x;
}

void emit (char *cmd, struct reg *r1, struct reg *r2, struct reg *r3)
{
     fprintf (out,"\t%s\t%c%c%d,%c%c%d,%c%c%d\n",
	      cmd,
	      0x25,r1->type,r1->num,
	      0x25,r2->type,r2->num,
	      0x25,r3->type,r3->num);
}

void emit2 (char *cmd, struct reg *r1, struct reg *r2)
{
     fprintf (out,"\t%s\t%c%c%d,%c%c%d\n",
	      cmd,
	      0x25,r1->type,r1->num,
	      0x25,r2->type,r2->num);
}

void flushall (struct reg *regs, char type)
{
     /*
      * flush all the registers of a given type to the stack
      */

     while (regs)
     {
	  if (regs->node)
	  {
	       if ((type == -1) || (regs->type == type))
		    flush(regs);
	  }
	       regs = regs->next;
     }
}

void flush (struct reg *r1)
{

     /*
      * flush a value in a register to it's proper home on the stack
      */
     int offset;

     if (r1->reserved)
     {
	  return;
     }

     if (!r1->node)
	  yyerror("internal error: register has no value");
     
     if (!(r1->dontflush))
     {
	  if (r1->node->offset)
	       offset = r1->node->offset;
	  else if (r1->node->type == LEAF_N)
	       offset = r1->node->val.lf->num.sym->offset;
	  else
	  {
	       sprintf (glob_errmsg,"internal error: no offset for node %x",
			r1->node);
	       yyerror (glob_errmsg);
	  }
	  if (r1->type == 'f')
	  {
	       fprintf (out,"\tst\t%c%c%d,%s\n", /* stf assembled from st */
			0x25, r1->type, r1->num, fp(offset));
	  }
	  else 
	  {
	       fprintf (out,"\tst\t%c%c%d,%s\n",
			0x25, r1->type, r1->num, fp(offset));
	  }
     }
     r1->node = NULL;
     r1->dontflush = 0;
}


void debug (char *section, char *msg, struct t_node *node)
{
     /*
      * print some assembly debugging stuff
      */

#ifdef DEBUG
     fprintf (out,"\t\t\t\t\t!%s(%x): %s\n",
	      section, node, msg);
#endif 
     return;
}
