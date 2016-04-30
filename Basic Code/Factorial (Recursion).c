#include<stdio.h>

long factorial(int);

int main()
{
  int number;
  long answer;

  printf("Enter an integer to find factorial\n");
  scanf("%d", &number);

  if (number < 0)
    printf("Negative integers are not allowed.\n");
  else
  {
    answer = factorial(number);
    printf("%d! = %ld\n", number, answer);
  }

  return 0;
}

long factorial(int number)
{
  if (number == 0)
    return 1;

  else
    return(number * factorial(number-1));
}
