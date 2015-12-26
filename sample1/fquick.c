/*
 * fquick.c
 * 
 * floating point quicksort
 * 
 * this program gets compiled by mcc
 */

void quicksort(int m, int n, float *a) {
  register int i, j;
  float v, x;
  
  if (n <= m) 
    return;
  i = m - 1; j = n; v = a[n];
  do {
    do i = i + 1; while(a[i] < v);
    do j = j - 1; while(a[j] > v);
    if (i < j){
      x = a[i]; a[i] = a[j]; a[j] = x;
    } else {
      x = a[i]; a[i] = a[n]; a[n] = x;
    }
  }while(i < j);
quicksort(m, j, a);
quicksort(i+1, n, a);
}

    
