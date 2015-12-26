/*
 * $Id: symbol.c,v 1.3 1996/09/17 21:15:27 berrym Exp $
 * $Log: symbol.c,v $
 * Revision 1.3  1996/09/17  21:15:27  berrym
 * Final submission.  Working version.
 *
 * Revision 1.2  1996/04/08  06:48:59  berrym
 * incorporated with parser; first working package
 * up to parsing declarations, not error free
 *
 * Revision 1.1  1996/02/23  14:36:55  berrym
 * Initial revision
 *
 *
 * mcc -- symbol.c
 * 
 * symbol table functions  (some of these functions
 * taken from the dragon book)
 *
 * Mike Berry
 * University of Pennsylvania
 * 23 Feb 1996
 */

#include "globals.h"

extern int lookups;
extern int linear_jumps;
extern int sym_debug;
int
dectest (struct entry **dec, struct table *symtab_cur)
{


     /*
      * if the lexer found the same ID at a
      * higher context, insert this one
      * at our current context
      */
     
     
     if ((*dec)->context != symtab_cur) 
     {
	  /*
	   * create a new symbol in the current context 
	   */

	  (*dec)->references--;	/* false lookup */
	  *dec = insert(symtab_cur,creat_symbol((*dec)->name, 1));  
     }
     
     /*
      * if this ID has already been defined,
      * report an error
      */
     
     if ((*dec)->references > 0)
	  return 0;
     return 1;
} 


char *
show_type (int type, int size, int offset)
{
     char *output;

     /*
      * we have a memory leak here, but its just 
      * a debugging functino, so we really don't
      * care 
      */

     output = (char *) malloc(50);

     sprintf(output,"e%d c%d t%d ex%d sz%d offset %d",
			    getbits(type,15,4),
			    getbits(type,11,4),
			    getbits(type,7,4),
			    getbits(type,3,4),
			    size, offset);
     return output;
}


/*
 * The design decision was between having multiple symbol
 * tables allocated as we enter new scope, or putting 
 * all the symbols into one table.  In the latter case, 
 * we would be performing a lot of pointer ops upon 
 * leaving a block, and search would be slightly slower.
 * In the former case, we deal with the malloc of the hash
 * table on every block (not as expensive, i think). 
 */

struct table *
creat_symtab (struct table *table_list)
{
     /*
      * create a new symbol table for a new scope and init it's
      * buckets to NULL
      */

     int x;
     struct table *last_ptr = NULL;

     /* 
      * depending on whether this function is sent the 
      * global symtab list head or the current scope ptr,
      * this while will loop multiple times or just once
      */

     while (table_list)		
     {
	  last_ptr=table_list;
	  table_list=table_list->next;
     }

     table_list = (struct table *) malloc (sizeof (struct table));
     table_list->prev=last_ptr;

     if (last_ptr)
	  last_ptr->next = table_list;

     for (x=0; x<PRIME; x++)
     {
	  table_list->bucket[x] = NULL;
     }

     return (table_list);
}

struct table *
pop_symtab (struct table *table_list, int free_syms)
{
     /*
      * remove the last symbol table in the list.  I can't currently
      * imagine any situation where you'd want to remove anything
      * but the last table in the list.
      */

     int x;
     struct entry *ptr, *save_ptr;
     struct table *last_ptr = NULL;

     /* 
      * depending on whether this function is sent the 
      * global symtab list head or the current scope ptr,
      * this while will loop multiple times or just once
      */

     while (table_list)		
     {
	  if (table_list->next)
	  {
	       table_list=table_list->next;
	  }
	  else
	       break;
     }

     last_ptr = table_list->prev;
     
     /*
      * free all the symbols in the table
      */

     if (free_syms)
     {
	  for (x=PRIME-1; x>=0; x--)
	  {
	       ptr=table_list->bucket[x];
	       while (ptr)
	       {
		    save_ptr = ptr;
		    ptr = ptr->next;
		    free(save_ptr);
	       }
	  }
     }

     free(table_list);	  
     if (last_ptr)
	  last_ptr->next = NULL;

     if (sym_debug)
	  printf ("sym: popping symbol table.  Returning to %x\n",last_ptr);
     return (last_ptr);
}

unsigned 
hash(char *s)
{
     /*
      * lou's hash function
      */
     unsigned h = 0, g;
     while (g = *s++) {
	  h = ( h << 4) + g;		/* shift left 4 and add next char */
	  if (g = h & 0xf0000000) 	/* if high b bits are set */
	  {
	       h = h ^ (g >> 24);	/* shift then left 24 and xor them in*/
	       h = h ^ g;		/* clear high 4 bits */
	  }
     }
     return h % PRIME;
}

void 
symtab_stats (struct table *table_list)
{
     int x, buckets=0, symbols=0, lev=0;
     struct entry *ptr;
     struct table *save_ptr = table_list;

     while (table_list)
     {

	  printf ("\n\nSymbol Table Statistics\n");
	  printf ("=======================\n");
	  
	  for (x=PRIME-1; x>=0; x--)
	  {
	       ptr=table_list->bucket[x];
	       if (ptr)
		    buckets++;
	       while (ptr)
	       {
		    symbols++;
		    ptr = ptr->next;
	       }
	  }
	  
	  printf ("number of buckets (used/total): ");
	  printf ("%d / %d\n\n",buckets,PRIME);
	  
	  printf ("number of symbols in table: ");
	  printf ("%d\n\n",symbols);
	  
	  printf ("total number of lookups: ");
	  printf ("%d\n\n",lookups);
	  
	  printf ("total number of linear search jumps: ");
	  printf ("%d\n\n",linear_jumps);

	  table_list=table_list->next;
     }
}

void 
symtab_dump (struct table *table_list)
{
     int x;
     struct entry *ptr, *ptr2;

     printf ("SYMBOL TABLE DUMP\n");
     while (table_list)
     {
	  printf ("TABLE SCOPE: %x\n",table_list);
	  for (x=PRIME-1; x>=0; x--)
	  {
	       ptr=table_list->bucket[x];
	       if (ptr)
		    printf ("bucket %d\n",x);
	       
	       while (ptr)
	       {
		    printf ("symbol: \"%s\" %s scope: %x\n",
			    ptr->name,
			    show_type(ptr->type, ptr->size, ptr->offset),
			    table_list);

		    if CHECK_BITS(e_fn,ptr->type,e_set)
			 ptr2=ptr->next_fn;
		    else
			 ptr2 = NULL;

		    while (ptr2)
		    {
			 printf ("       symbol: \"%s\" e%d c%d t%d ex%d sz%d offst %d scope: none\n",
				 ptr2->name,
				 getbits(ptr2->type,15,4),
				 getbits(ptr2->type,11,4),
				 getbits(ptr2->type,7,4),
				 getbits(ptr2->type,3,4),
				 ptr2->size,ptr2->offset);
			 ptr2 = ptr2->next_fn; 
		    }
		    
		    ptr = ptr->next;
	       }
	  }
	  table_list=table_list->prev;
     }
}

HalfWord getbits(HalfWord x, int p, int n)
{
     return (x >> (p+1-n)) & ~(~0 << n);
}

struct entry *
creat_symbol (char *s, int size)
{
     struct entry * p;
     HalfWord t=0;

     p = (struct entry * ) malloc (sizeof (struct entry));
     p->name = strdup(s);

     t = 0;

     p->offset = 0;
     p->type = t;	  
     p->references = 0;		/* init to 1 */
     p->next_fn = NULL;
     p->next_par = NULL;
     p->next_const = NULL;
     p->size = size;
     p->next = NULL;

     return p;
}

int get_offset(struct table *symtab)
{
     /*
      * get the current offset furthest from the frame pointer 
      */

     
     int min=0;
     int x;
     struct entry *ptr;
     while (symtab)
     {
	  for (x=PRIME-1; x>=0; x--)
	  {
	       ptr=symtab->bucket[x];
	       while (ptr)
	       {
		    if ((ptr->offset) && (ptr->offset < min))
			 min = ptr->offset;
			 
		    ptr=ptr->next;
	       }
	  }
	  symtab=symtab->prev;
     }
     return min;
}

int add_tmp_offset (HalfWord type, struct table *symtab)
{
     int current_offset;
     int offset = 0;
     HalfWord t1;

     current_offset = get_offset(symtab) + glob_tmp_offset;

     offset = add_offset(type, 1, symtab, glob_tmp_offset);
     glob_tmp_offset += (offset - current_offset);

     return offset;


}

int add_offset(HalfWord type, int size, struct table *symtab, int extra)
{


     int current_offset;
     int offset = 0;
     HalfWord t1;

     if (!type)
	  yyerror ("internal error: tried to alloc space with no type.");

     current_offset = get_offset(symtab) + extra;
     if (CHECK_BITS(type,e_fn,e_set)) /* no offset for fn */
	  return 0;
     t1 = ISO_BITS(type, t_set);
     switch(t1) {
     case t_int:
	  offset = (current_offset-(4*size)) & -4; /* word aligned */
	  break;
     case t_char:
	  offset = (current_offset-(4*size)) & -2; /* halfword aligned */
	  break;
     case t_float:
	  offset = (current_offset-(4*size)) & -4; /* word aligned */
	  break;
     case t_void:
	  offset = (current_offset-(4*size)) & -4; /* word aligned */
	  break;
     };
     if (offset < glob_offset)
	  glob_offset = offset;	/* used for the save sp cmd */
     return offset;

}

struct entry *
insert (struct table *symtab,  
	struct entry *p)
{

     unsigned n;
     struct entry *save_ptr=p;
     
     while (p)
     {
	  

	  n = hash(p->name);
	  
	  p->context = symtab; /* set the context of this entry */	       

	  /*
	   * insert at head of bucket 
	   */
	  
	  p->next = symtab->bucket[n];
	  symtab->bucket[n] = p;
	  
	  if (sym_debug){
	       printf ("sym:insert %s e%d c%d t%d ex%d sz%d scope: %x\n",
		       p->name,
		       getbits(p->type,15,4),
		       getbits(p->type,11,4),
		       getbits(p->type,7,4),
		       getbits(p->type,3,4),
		       p->size,symtab);
	       
	  }
	  
	  p=p->next_par;
     }
     return save_ptr;
}

struct entry * 
lookup (struct table *symtab, char *s)
{

     struct entry *p;
     unsigned n;
     
     lookups++;
     n = hash(s);

     while (symtab)
     {
	  p = symtab->bucket[n];     
	  while (p) 
	  {
	       if (!strcmp(p->name , s)) 
	       {
		    p->references++;
		    return p;
	       }
	       else
	       {
		    linear_jumps++;
		    p = p->next;
	       }
	  }
	  symtab=symtab->prev; /* always check upper contexts */

     }
     return NULL;
}


void add_const (struct entry **list, struct entry *cnst)
{

     /*
      * add cnst (a symbol table entry) to the 
      * list of constant symbols (list)
      */

     struct entry *savestart = NULL;
     struct entry *saveptr = NULL;
     struct entry *tmpentry;
     
     if (cnst->references > 0)
     {
/*	  printf ("add_const: too many refs (%d) on %s\n",cnst->references,cnst->name);*/
	  return;		/* we only want to add the cnst once */
     }

     if (*list)
	  savestart = *list;

     while (*list)
     {

	  saveptr = *list;
	  *list = (*list)->next_const;
     }
     
     cnst->next_const = NULL;

     if (!saveptr)
	  *list = cnst;
     else
	  saveptr->next_const = cnst;
     cnst->offset = glob_const_offset++;
     
     if (savestart)
	  *list = savestart;

     

}
