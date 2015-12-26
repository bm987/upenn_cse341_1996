/*
 * fmquick.c
 * 
 * wrapper for fquick.c
 * 
 * this program gets compiled by gcc
 */

#include <stdio.h>

#define NELEMS(a) ((int)(sizeof (a)/ sizeof((a)[0])))
float a[] = { 2000, -3, 99.1, 14.54, 73, 3.14, 1, 1, 100 };

void quicksort(int m, int n, float * a);

main() {
  int i;
  for ( i = 0; i < NELEMS(a); i++)
    printf(" %-2.2f", a[i]);
  putchar('\n');
  quicksort(0, NELEMS(a) - 1, a);
  for ( i = 0; i < NELEMS(a); i++)
    printf(" %-2.2f", a[i]);
  putchar('\n');
}

