/*
 * ssort2.c
 * 
 * shell sort -- main routine
 * 
 * this will be compiled by mcc
 */

void sort (int n, int * v, int *count) {
  register int gap, i, j;
  register int temp;


  for ( gap = n / 2; gap > 0; gap = gap / 2)
    for ( i = gap; i < n; i++)
      for( j = i - gap; j >= 0 && v[j] > v[j + gap]; j = j - gap)
	{
	  temp = v[j];
	  v[j] = v[j + gap];
	  v[j+ gap] = temp;
	  *count = *count + 1;
	}
}

