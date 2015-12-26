/*
 * factorial-sum.c
 * 
 * call factorial to compute the sum of multiple factorials
 *
 * compiled by mcc
 */


int factorial (int x);		/* also compiled by mcc */

int allfact (int top)
{

     int x;			/* tmpval: factorial */
     int sum;			/* running sum */

     sum = 0;
     
     for (x = 1; x <= top; x++)
     {
	  sum = sum + factorial(x);
     }

     return sum;
}
