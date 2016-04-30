#include<stdio.h>

int Fibonacci(int);

main()
{
   int number, i;

   printf("Number To Find Fibonacci Of:\n");
   scanf("%d",&number);

   for ( i = 1 ; i <= number ; i++ )
      printf("Fib(%d) = %d\n", i, Fibonacci(i));

   return 0;
}

int Fibonacci(int number)
{
   if ( number == 0 )
      return 0;

   else if ( number == 1 )
      return 1;

   else
      return ( Fibonacci(number-1) + Fibonacci(number-2) );
}
