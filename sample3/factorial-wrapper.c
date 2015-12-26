/*
 * factorial-wrapper.c
 *
 * wrapper for factorial routines
 *
 * compiled by gcc
 */


#include<stdio.h>

int allfact();			/* factorial-sum.c */

main ()
{
     printf ("Factorial Sum from 1 to 20 is %d\n",
	     allfact(20));
}
