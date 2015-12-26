/*
 * ssortmain.c
 * 
 * wrapper for shell sort routines
 * 
 * this program will be compiled by gcc
 */


void sort (int n, int * v, int *count);
void init(int *ary);

main() {
     int ary[10];
     int i;
     int count;
     
     count = 0;
     init(ary);
     for ( i = 0; i < 10; i++)
	  printf("%4d", ary[i]);
     putchar('\n');
     sort ( 10, ary, &count );
     for ( i = 0; i < 10; i++)
	  printf("%4d", ary[i]);
     putchar('\n');
     printf ("Sort went through %d iterations\n",count);
     return 0;
}
