/*
 * $Id: tree.c,v 1.1 1996/09/17 21:15:35 berrym Exp $
 * $Log: tree.c,v $
 * Revision 1.1  1996/09/17  21:15:35  berrym
 * Initial revision
 *
 *
 * mcc -- syntax tree functions
 *
 * Mike Berry
 * University of Pennsylania
 * 8 Apr 1996
 */

#include "globals.h"
#include "mcc.tab.h"

void
dump_tree (struct t_node *tree, int indent)
{
     /*
      * visually show the contents of a syntax tree 
      */

     int x;
     struct leaf *l;

     while (tree)
     {

	  for (x=0; x<indent; x++) /* indent */
	       printf (" ");

	  switch (tree->type)
	  {
	  case T_N:
	       printf ("T_N (%x): Token is %c (%d): %s \n",tree,tree->val.token,
		       tree->val.token,
		       show_type(tree->subtype, tree->size,tree->offset));
	       if (tree->LEFT)
		    dump_tree (tree->LEFT, indent+2);
	       if (tree->RIGHT)
		    dump_tree (tree->RIGHT, indent+2);
	       break;
	  case U_N:
	       printf ("UNARY NODE (%x):  Token is %c (%d): %s \n",tree,tree->val.token,
		       tree->val.token,
		       show_type(tree->subtype, tree->size, tree->offset));
	       if (tree->CHILD)
		    dump_tree (tree->CHILD, indent+2);
	       break;
	  case LEAF_N:
	       l = tree->val.lf;
	       printf ("LEAF:  ");

	       printf ("%s ",l->num.sym->name);

	       printf (": %s \n",
		       show_type(tree->subtype, tree->size,0));


	       break;
	  case IF_N:
	       printf ("IF_N: \n");
	       if (tree->EXPR)
		    dump_tree (tree->EXPR, indent+2);
	       if (tree->STMT)
		    dump_tree (tree->STMT, indent+2);
	       if (tree->ELS)
		    dump_tree (tree->ELS, indent+2);
	       break;
	  case FOR_N:
	       printf ("FOR_N: \n");
	       if (tree->INIT)
		    dump_tree (tree->INIT, indent+2);
	       if (tree->EXPR)
		    dump_tree (tree->EXPR, indent+2);
	       if (tree->INCR)
		    dump_tree (tree->INCR, indent+2);
	       if (tree->STMT)
		    dump_tree (tree->STMT, indent+2);
	       
	       break;
	  case DO_N:
	       printf ("DO_N:");
	       if (tree->EXPR)
		    dump_tree (tree->EXPR, indent+2);
	       if (tree->STMT)
		    dump_tree (tree->STMT, indent+2);
	       break;
	  };


	  tree = tree->next;
	  if (tree)
	       printf ("\n\n");
     }

}
      

struct t_node *
mknod (int type)
{

     /*
      * allocate a skeleton node and return its potiner
      */

     struct t_node *ptr;

     ptr = (struct t_node *) malloc(sizeof(struct t_node));

     if (type == LEAF_N)
     ptr->val.lf = (struct leaf *) malloc(sizeof(struct leaf));

     ptr->offset = 0;
     ptr->subtype = 0;
     ptr->type = type;
     ptr->next = NULL;
     ptr->n1 = NULL;
     ptr->n2 = NULL;
     ptr->n3 = NULL;
     ptr->n4 = NULL;

     return ptr;
}


/* freetree (struct t_node *head) */


int typecheck_binary (struct t_node **out,
		      struct t_node *in_1, struct t_node *in_2,
		      int in_token)
{
     /* 
      * type-checking: 
      * int, char -> float
      * int, char -> ptr
      * same -> same
      * else : error
      */
     
     HalfWord t1, t2, c1, c2, subtype, type_sca;
     struct t_node *n1, *n2;
     int ptr_scalar;          /* flag   */

     subtype = 0;

     if (!(in_1->subtype))
	  yyerror("internal error: left side of operand doesn't have a type!");

     if (!(in_2->subtype))
	  yyerror("internal error: left side of operand doesn't have a type!");
	  
     t1=ISO_BITS(in_1->subtype, t_set);
     t2=ISO_BITS(in_2->subtype, t_set);
     c1=ISO_BITS(in_1->subtype, c_set);
     c2=ISO_BITS(in_2->subtype, c_set);

     /* if types are same, go ahead */
     if (((t1 == t2) && (c1 == c2) && (c1 == c_scalar))
	 || ((c1 == c2) && (c1 == c_pointer)))
	  /* if t and c are same, or if c1 and */
	  /* c2 are ptrs, then go ahead */
	  
     {
	  n1 = in_1;
	  n2 = in_2;
	  subtype = in_1->subtype; /* change e_? */
     }
     else  if ((c1 == c2) && (c1 == c_scalar))
     {
	  /* both scalars but of */
	  /* different type */
	  switch (t1)
	  {
	  case t_int:
	       if (t2 == t_char)
	       {
		    n2 = mknod(U_N);
		    n2->val.token = CTOI;
		    n2->CHILD = in_2;
		    SET_BITS(n2->subtype,c_scalar,c_set);
		    SET_BITS(n2->subtype,t_int,t_set);
		    n1 = in_1;
		    subtype = in_1->subtype;
		    n2->offset =
			 add_tmp_offset(n2->subtype,symtab_cur);
	       }
	       if (t2 == t_float)
	       {
		    n1 = mknod(U_N);
		    n1->val.token = ITOF;
		    n1->CHILD = in_1;
		    SET_BITS(n1->subtype,c_scalar,c_set);
		    SET_BITS(n1->subtype,t_float,t_set);
		    n2 = in_2;
		    subtype = in_2->subtype;
		    n1->offset =
			 add_tmp_offset(n1->subtype,symtab_cur);
	       }
	       break;
	  case t_char:
	       if (t2 == t_int)
	       {
		    n1 = mknod(U_N);
		    n1->val.token = CTOI;
		    n1->CHILD = in_1;
		    SET_BITS(n1->subtype,c_scalar,c_set);
		    SET_BITS(n1->subtype,t_int,t_set);
		    n2 = in_2;
		    subtype = in_2->subtype;
		    n1->offset =
			 add_tmp_offset(n1->subtype,symtab_cur);
	       }
	       if (t2 == t_float)
	       {
		    n1 = mknod(U_N);
		    n1->val.token = CTOF;
		    n1->CHILD = in_1;
		    SET_BITS(n1->subtype,c_scalar,c_set);
		    SET_BITS(n1->subtype,t_float,t_set);
		    n2 = in_2;
		    subtype = in_2->subtype;
		    n1->offset =
			 add_tmp_offset(n1->subtype,symtab_cur);
	       }
	       break;
	  case t_float:
	       if (t2 == t_int)
	       {
		    n2 = mknod(U_N);
		    n2->val.token = ITOF;
		    n2->CHILD = in_2;
		    SET_BITS(n2->subtype,c_scalar,c_set);
		    SET_BITS(n2->subtype,t_float,t_set);
		    n1 = in_1;
		    subtype = in_1->subtype;
		    n2->offset =
			 add_tmp_offset(n2->subtype,symtab_cur);
	       }
	       if (t2 == t_char)
	       {
		    n2 = mknod(U_N);
		    n2->val.token = CTOF;
		    n2->CHILD = in_2;
		    SET_BITS(n2->subtype,c_scalar,c_set);
		    SET_BITS(n2->subtype,t_float,t_set);
		    n1 = in_1;
		    subtype = in_1->subtype;
		    n2->offset =
			 add_tmp_offset(n2->subtype,symtab_cur);
	       }
	       break;
	  };
     }
     /* otherwise we've got a */
     /* pointer in here somewhere */
     else if ((c1 == c_pointer) || 
	      (c2 == c_pointer))
     {
	  if (c1 == c_pointer)
	  {
	       ptr_scalar = 1;
	       type_sca = t2;
	  }
	  else
	  {
	       ptr_scalar = 0;
	       type_sca = t1;
	  }
	  
	  
	  switch (type_sca)
	  {
	  case t_int:
	       if (ptr_scalar)
	       {
		    n2 = mknod(U_N);
		    n2->val.token = ITOP;
		    n2->CHILD = in_2;
		    SET_BITS(n2->subtype,c_pointer,c_set);
		    SET_BITS(n2->subtype,t_int,t_set);
		    n1 = in_1;
		    subtype = in_1->subtype;
		    n2->offset =
			 add_tmp_offset(n2->subtype,symtab_cur);
	       }
	       else
	       {
		    n1 = mknod(U_N);
		    n1->val.token = ITOP;
		    n1->CHILD = in_1;
		    SET_BITS(n1->subtype,c_pointer,c_set);
		    SET_BITS(n1->subtype,t_int,t_set);
		    n2 = in_2;
		    subtype = in_2->subtype;
		    n1->offset =
			 add_tmp_offset(n1->subtype,symtab_cur);
	       }
	       break;
	  case t_char:
	       if (ptr_scalar)
	       {
		    n2 = mknod(U_N);
		    n2->val.token = CTOP;
		    n2->CHILD = in_2;
		    SET_BITS(n2->subtype,c_pointer,c_set);
		    SET_BITS(n2->subtype,t_char,t_set);
		    n1 = in_1;
		    subtype = in_1->subtype;
		    n1->offset =
			 add_tmp_offset(n1->subtype,symtab_cur);
	       }
	       else
	       {
		    n1 = mknod(U_N);
		    n1->val.token = CTOP;
		    n1->CHILD = in_1;
		    SET_BITS(n1->subtype,c_pointer,c_set);
		    SET_BITS(n1->subtype,t_int,t_set);
		    n2 = in_2;
		    subtype = in_2->subtype;
		    n2->offset =
			 add_tmp_offset(n2->subtype,symtab_cur);
	       }
	       break;
	  case t_float:
	       sprintf (glob_errmsg,"You are not permitted to op a float to a pointer %x %x",
			in_1->subtype,in_2->subtype);
	       yyerror(glob_errmsg);
	       break;
	       
	  case t_void:
	       yyerror("There is no such thing as a void scalar");
	       break;
	       
	  };
     }
     
     /*
      * Now make the top node with its operand
      */
     
     *out=mknod(T_N);
     (*out)->val.token = in_token; 
     (*out)->LEFT = n1; 
     (*out)->RIGHT = n2; 
     (*out)->subtype = subtype;

     if (!subtype)
	  yyerror("internal error: typecheck binary never added subtype");

     (*out)->offset =
	  add_tmp_offset((*out)->subtype,symtab_cur);
     return 1;
}

int check_params (struct t_node *incoming, struct entry *proto)
{
     /*
      * check the argument list in a tree with
      * the prototype's variable list
      */

     /*
      * some of the most pathetic code in the compiler;
      * i think it'll make it through, though
      */

     struct t_node *tmp_node;
     HalfWord c1, c2, t1, t2;
     struct t_node *savenext;

     if ((!incoming) && (!proto))
	  return 1;		/* no arguments */
     while (incoming)
     {
	  savenext = incoming->next;
	  if (proto)
	  {
	       c1 = ISO_BITS(incoming->subtype,  c_set);
	       c2 = ISO_BITS(proto->type,  c_set);
	       t1 = ISO_BITS(incoming->subtype,  t_set);
	       t2 = ISO_BITS(proto->type,  t_set);

	       if (c1 == c_array) /* this is a design flaw; array and */
				  /* pointer are equiv. here; make it so */
		    c1 = c_pointer;
	       if (c2 == c_array)
		    c2 = c_pointer;
	       
	       if (c1 != c2)
		    return 0;	/* pointer/scalar -- not resolvable */
	       else if (c1 == c_scalar)
	       {
		    if (t1 != t2)
		    {
			 if ((t1 == t_float) && (t2 == t_int))
			 {
			      /*
			       * make a new node just like
			       * incoming; change incoming
			       * to point to the new node
			       */

			      tmp_node = mknod(incoming->type);
			      tmp_node->subtype = incoming->subtype;
			      tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
			      incoming->type = U_N;			      
    			      incoming->val.token = FTOI;
			      incoming->CHILD = tmp_node;
			      SET_BITS(incoming->subtype, t_int, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);

			 }
			 if ((t1 == t_float) && (t2 == t_char))
			 {
			      /*
			       * make a new node just like
			       * incoming; change incoming
			       * to point to the new node
			       */

			      tmp_node = mknod(incoming->type);
			      tmp_node->subtype = incoming->subtype;
			      tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
			      incoming->type = U_N;			      
    			      incoming->val.token = FTOC;
			      incoming->CHILD = tmp_node;
			      SET_BITS(incoming->subtype, t_char, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);
			 }
			 if ((t1 == t_int) && (t2 == t_float))
			 {
			      /*
			       * make a new node just like
			       * incoming; change incoming
			       * to point to the new node
			       */

			      tmp_node = mknod(incoming->type);
			      tmp_node->subtype = incoming->subtype;
			      tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
			      incoming->type = U_N;			      
    			      incoming->val.token = ITOF;
			      incoming->CHILD = tmp_node;
			      SET_BITS(incoming->subtype, t_float, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);
			 }
			 if ((t1 == t_int) && (t2 == t_char))
			 {
                              /*
                               * make a new node just like
                               * incoming; change incoming
                               * to point to the new node
                               */

                              tmp_node = mknod(incoming->type);
                              tmp_node->subtype = incoming->subtype;
                              tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
                              incoming->type = U_N;
                              incoming->val.token = ITOC;
                              incoming->CHILD = tmp_node;
                              SET_BITS(incoming->subtype, t_char, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);
			 }
			 if ((t1 == t_char) && (t2 == t_float))
			 {
                              /*
                               * make a new node just like
                               * incoming; change incoming
                               * to point to the new node
                               */

                              tmp_node = mknod(incoming->type);
                              tmp_node->subtype = incoming->subtype;
                              tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
                              incoming->type = U_N;
                              incoming->val.token = CTOF;
                              incoming->CHILD = tmp_node;
                              SET_BITS(incoming->subtype, t_float, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);
			 }
			 if ((t1 == t_char) && (t2 == t_int))
			 {
                              /*
                               * make a new node just like
                               * incoming; change incoming
                               * to point to the new node
                               */

                              tmp_node = mknod(incoming->type);
                              tmp_node->subtype = incoming->subtype;
                              tmp_node->val = incoming->val;
			      copy_children(tmp_node, incoming);
                              incoming->type = U_N;
                              incoming->val.token = CTOI;
                              incoming->CHILD = tmp_node;
                              SET_BITS(incoming->subtype, t_int, t_set);
			      incoming->offset =
				   add_tmp_offset(incoming->subtype,symtab_cur);
			 }
		    }			  
	       }
	       /* else they're both pointers, that's o.k. */

	       incoming = savenext;
	       proto = proto->next_fn;
	  }
	  else
	  {
	       yywarning("more parameters to fn than defined in its prototype");
	       return 1;
	  }
     }
     if (proto)
	  return 0;
     
     return 1;
}

void copy_children (struct t_node *t1, struct t_node *t2)
{
     t1->next = t2->next;
     t1->n1 = t2->n1;
     t1->n2 = t2->n2;
     t1->n3 = t2->n3;
     t1->n4 = t2->n4;
}
