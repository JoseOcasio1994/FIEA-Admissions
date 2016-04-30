#include <stdio.h>
#include <string.h>

int main()
{
    char a[100], b[100];
    int length;

    printf("Enter the first string\n");
    gets(a);

    printf("Enter the second string\n");
    gets(b);

    // String Length
    length = strlen(a);
    printf("Length of %s is %d\n\n", a, length);

    // String Compare
    if (strcmp(a,b) == 0)
        printf("Entered strings are equal.\n\n");
    else
        printf("Entered strings are not equal.\n\n");

    // String Copy
    strcpy(a, b);
    printf("String 1: %s\n", a);
    printf("String 2: %s\n\n", b);

    // String Concatenation
    strcat(a,b);
    printf("Concatenated String Is %s\n",a);

    return 0;
}
