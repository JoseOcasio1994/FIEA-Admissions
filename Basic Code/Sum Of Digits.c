#include <stdio.h>

int main()
{
   int Number, Temp, sum = 0, remainder;

   printf("Enter an integer\n");
   scanf("%d", &Number);

   Temp = Number;

   while (Temp != 0)
   {
      remainder = Temp % 10;
      sum += remainder; // Increase Sum By Right-Most Digit
      Temp  /= 10;  // Moves Number Over 1 Decimal (Ex: 123 -> 12.3 -> 12
   }

   printf("Sum of digits of %d = %d\n", Number, sum);

   return 0;
}
