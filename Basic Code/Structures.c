#include <stdio.h>

struct student
{
    char First[50];
    char Last[50];
    float GPA;
};

int main()
{
    struct student s;

    printf("Enter information of students:\n\n");

    printf("Enter First Name: ");
    scanf("%s",s.First);

    printf("Enter Last Name: ");
    scanf("%s",s.Last);

    printf("Enter GPA: ");
    scanf("%f",&s.GPA);

    printf("\nDisplaying Information\n");
    printf("Name: %s %s\n", s.First, s.Last);
    printf("Marks: %.2f\n",s.GPA);

    return 0;
}
