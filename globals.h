/*
 * $Id: globals.h,v 1.4 1996/09/17 23:51:11 berrym Exp $
 * $Log: globals.h,v $
 * Revision 1.4  1996/09/17  23:51:11  berrym
 * added debug fn
 *
 * Revision 1.3  1996/09/17  21:13:59  berrym
 * Final submission.  Working version.
 *
 * Revision 1.2  1996/04/08  06:49:37  berrym
 * incorporated with parser; first working package
 * up to parsing declarations, not error free
 *
 * Revision 1.1  1996/02/23  14:37:03  berrym
 * Initial revision
 *
 *
 * mcc -- globals.h
 *
 * Global variables and such.
 * 
 * Mike Berry
 * University of Pennsylvania
 * 23 Feb 1996
 */

#include<string.h>
#include<stdio.h>
#include<assert.h>

typedef unsigned char Byte;         /* 8 bit */
typedef unsigned short HalfWord;    /* 16 bit */
typedef unsigned long Word;	    /* 32 bit */
typedef unsigned long long DblWord; /* 64 bit */

/*
 * global defines, structs
 */

#define SET_BITS(TARGET, BITS, CLEAR) (TARGET = ((TARGET ^ CLEAR) & TARGET) | BITS)

#define CHECK_BITS(TARGET, BITS, CLEAR) ((~(~CLEAR | TARGET)) == (~(~CLEAR | BITS)))

#define ISO_BITS(BITS, CLEAR) (~(~CLEAR | BITS)) ^ (CLEAR)

#define MAX(A,B) (A > B) ? A : B
#define MIN(A,B) (A < B) ? A : B

#define CURSYM_ONLY 0
#define ALLSYMS     1


#define LEFT n1
#define INIT n1
#define RIGHT n2
#define CHILD n1
#define STMT n3
#define EXPR n2
#define INCR n4
#define ELS n4

#define PRIME 211

/*
 * we use a single HalfWord to store all the type information for a symbol
 */

#define e_unk   0x0000		/* << 12 */
#define e_fn    0x1000
#define e_var   0x2000
#define e_const 0x3000
#define e_set   0xf000

#define t_unk   0x0000		/* << 4 */
#define t_int   0x0010
#define t_char  0x0020
#define t_float 0x0030
#define t_void  0x0040
#define t_set   0x00f0

#define c_unk     0x0000	/* << 8 */
#define c_scalar  0x0100
#define c_array   0x0200
#define c_pointer 0x0300
#define c_set     0x0f00

#define ex_unk      0x0000	/* << 0 */
#define ex_extern   0x0001
#define ex_register 0x0002
#define ex_set      0x000f

/*
 * symbol table structures
 */

struct table {
     struct entry *bucket[PRIME]; /* pointer to array of hash buckets */
     struct table *next;	  /* next symbol table */
     struct table *prev;	  /* previous symbol table */

     /* 
      * need doubly linked list so traverse up
      * during a lookup
      */
};


struct entry {
  char *name;
  HalfWord type;
  int size;
  int width;
  int offset;			/* offset from frame pointer */
  int   references;
  struct table *context;	/* context level identifier */
  struct entry *next_par;	/* temporary ptr: keep list of symbols that */
				/* appear in a list: i.e., int a,b,c */
  struct entry *next_fn;	/* pointer to param_list for function decl */
  struct entry *next_const;	/* pointer to next in constant list */
  struct entry *next;
};


/*
 * syntax tree structures
 */


typedef enum n_type {
     T_N,
     LEAF_N,
     IF_N,
     FOR_N,
     DO_N,
     U_N
} n_type;

/* 
 * some of the pieces in the tree structure are overkill and not used
 */

struct leaf {
     union {
	  int i;		/* we might use these for constants to */
				/* speed things up, but we're going to */
				/* ignore them for now */
	  float f;		
	  char c;
	  
	  struct entry *sym;
     } num;

	  struct t_node *args;	
};



struct t_node {
     n_type type;
     int subtype;
     int offset;		/* temporary values have a frame pointer location */

     union {
	  int token;
	  struct leaf *lf;
     } val;

     int size;			/* not used */

     struct t_node *next;
     struct t_node *n1;		/* left   init  child        */
     struct t_node *n2;		/* right  expr               */
     struct t_node *n3;		/*        stmt               */
     struct t_node *n4;		/*        incr  else         */

};

/*
 * structure for register allocation
 */

struct reg {
     char type;			/* 'o', 'i', etc. */
     int num;
     struct t_node *node;
     struct reg *next;
     int reserved;		/* don't ever use, i.e., %fp */
     int dontflush;		/* no need to flush back to stack, i.e., const */
};

struct reg *glob_regs;
struct reg *g0_reg;
int labelno;



/*
 * global vars 
 */

char glob_str[16];		/* for small assembly lng strs */
int glob_offset;
struct entry *glob_mainfn;
struct t_node *tree_root;	/* root of syntax tree */
FILE *out;
int glob_tmp_offset;
int glob_parm_offset;
int glob_const_offset;
struct table *symtab_l, *symtab_cur, *tmptab; /* symbol table lists */
struct entry *glob_const_list;
char glob_errmsg[256];

/*
 * function prototypes
 */


struct table *creat_symtab (struct table *table_list);

struct table *pop_symtab (struct table *table_list, int free_syms);

unsigned hash(char *s);

void symtab_stats (struct table *table_list);

void symtab_dump (struct table *table_list);

HalfWord getbits(HalfWord x, int p, int n);

struct entry *creat_symbol (char *s, int size);

struct entry *insert (struct table *symtab,  
		      struct entry *p);

struct entry *lookup (struct table *symtab, char *s);

struct t_node *mknod (int token);
void dump_tree (struct t_node *tree, int indent);

char *show_type (int type, int size,  int offset);


int dectest (struct entry **dec, struct table *symtab_cur);
int typecheck_binary (struct t_node **out,
		      struct t_node *in_1, struct t_node *in_2,
		      int in_token);
int check_params (struct t_node *incoming, struct entry *proto);
void yywarning(const char *msg);
void copy_children (struct t_node *t1, struct t_node *t2);
int add_offset(HalfWord type, int size, struct table *symtab, int extra);
void codegen(struct t_node *root);
int add_tmp_offset (HalfWord type,struct table *symtab);
void code_statements(struct t_node *tree, struct reg *regs);
void code_statement(struct t_node *tree, struct reg *regs);

void reginit (struct reg **regs);
struct reg * search (struct reg *regs, struct t_node *node);
struct reg * regalloc (struct reg *regs, struct t_node *node, 
		       int requirements,char type, int refresh_flag, int
		       regnum);
void refresh (struct reg *regs);
int amountused (struct reg *regs, char type);
void emit (char *cmd, struct reg *r1, struct reg *r2, struct reg *r3);

void flushall (struct reg *regs, char type);
void flush (struct reg *r1);
void emit2 (char *cmd, struct reg *r1, struct reg *r2);
int find_offset (struct t_node *node);
int get_offset(struct table *symtab);
void pull_in_registers (struct entry *fn);
char *fp (int offset);
void code_call (int arg_number, struct t_node *parameter_list, struct reg
	      *regs);
void code_const (struct entry *list);
void add_const (struct entry **list, struct entry *cnst);
void debug (char *section, char *msg, struct t_node *node);
