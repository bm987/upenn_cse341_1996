/*
 * sample.c
 * 
 * a silly program demonstrating various control structures and
 * expressions.  doesn't produce any useful output.
 *
 * compiled by mcc
 */


int sample(int a, int b)
{

     int x,y,z;
     int *ptr;
     float f,g;

     for (y = a; y < b; y = y * 2)
     {
	  z = y;
	  do
	  {
	       z++;
	  } while (z >= 0);

	  if ((y > 100) || (y < 50))
	       return y;

	  while (y != 0)
	       y--;
	  f = g + (y + z) - x;

	  *ptr = &a;
	  if (*ptr == 100)
	       return *ptr;
     }
}

	  
