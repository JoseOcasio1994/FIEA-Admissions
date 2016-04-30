// Jose Ocasio

#include <stdio.h>
#include <string.h>

#define MAX_STACK_HEIGHT  2000
#define MAX_CODE_LENGTH  500
#define MAX_LEXI_LEVELS 3
const char *Operations[] = {"Lit", "Opr", "Lod", "Sto", "Csl", "Inc", "Jmp", "Jpc", "Sio"};

struct instruction
{
    int OP;
    int L;
    int M;
    int Line;
};

// Main Variables
int Stack_Pointer = 0, Base_Pointer = 1, Program_Counter = 0;
int Stack[MAX_STACK_HEIGHT];
struct instruction Code[MAX_CODE_LENGTH];
struct instruction Instruction_Register;
FILE *fp, *fp_out;

// Prototypes
int base(int level, int b);
void LIT(int m);
void OPR(int m);
void LOD(int l, int m);
void STO(int l, int m);
void CAL(int l, int m);
void INC(int m);
void JMP(int m);
void JPC(int m);
void SIO(int m);

int main()
{
    // File and Loop Variables
    char filename[50], line[10];
    int i = 0, j;

    // Initialize Stack
    Stack[1] = 0;
    Stack[2] = 0;
    Stack[3] = 0;

    // Find File To Scan
    fp = fopen ("mcode.txt", "r");
    fp_out = fopen ("stacktrace.txt", "w");
    if(fp == NULL)
    {
        fclose(fp);
        printf("File Not Found Or Does Not Exist. Closing Program.\n");
    }

    // Print Header
    fprintf(fp_out, "\n Line\tOP\tL\tM");

    // Scan File
   while(fgets(line, 20, fp) != NULL)
    {
        sscanf(line, "%d %d %d", &Code[i].OP, &Code[i].L, &Code[i].M);
        Code[i].Line = i;

        // Print Copy Of File
        fprintf(fp_out, "\n  %d\t%s\t%d\t%d", i, Operations[Code[i].OP - 1], Code[i].L, Code[i].M);

        i++;
    }

    // Print Header For Executed Instructions
    fprintf(fp_out, "\n\n\t\t\t\tPC      BP      SP      Stack\n        Initial Values \t\t0       1       0");

    // Loop Through And Execute Each Instruction
    while( i > 0)
    {
        Instruction_Register = Code[Program_Counter];

        // Print Initial Values
        fprintf(fp_out, "\n  %d \t%s \t%d \t%d", Code[Program_Counter].Line, Operations[Code[Program_Counter].OP-1], Code[Program_Counter].L, Code[Program_Counter].M);

        Program_Counter++;

        if(Instruction_Register.OP == 1)
            LIT(Instruction_Register.M);

        if(Instruction_Register.OP == 2)
            OPR(Instruction_Register.M);

        if(Instruction_Register.OP == 3)
            LOD(Instruction_Register.L,Instruction_Register.M);

        if(Instruction_Register.OP == 4)
            STO(Instruction_Register.L,Instruction_Register.M);

        if(Instruction_Register.OP == 5)
            CAL(Instruction_Register.L,Instruction_Register.M);

        if(Instruction_Register.OP == 6)
            INC(Instruction_Register.M);

        if(Instruction_Register.OP == 7)
            JMP(Instruction_Register.M);

        if(Instruction_Register.OP == 8)
            JPC(Instruction_Register.M);

        if(Instruction_Register.OP == 9 && Instruction_Register.M != 2)
            SIO(Instruction_Register.M);

        // Print PC, BP, and SP
        fprintf(fp_out, "\t%d\t%d\t%d\t", Program_Counter, Base_Pointer, Stack_Pointer);

        for(j = 1; j < Stack_Pointer + 1; j++)
        {
            if(Base_Pointer == j && Base_Pointer > 1)
                fprintf(fp_out, "| ");

            fprintf(fp_out, "%d ", Stack[j]);
        }

        // HALT
        if(Instruction_Register.OP == 9 && Instruction_Register.M == 2)
            i = 0;

        // next Instruction
        i--;
    }

    fclose(fp);
    fclose(fp_out);

    return 0;
}


// Find Base L Levels Down
int base(int level, int b)
{
    while (level > 0)
    {
        b = Stack[b + 2];
        level--;
    }
 return b;
}

// Literals
void LIT(int m)
{
    Stack_Pointer++;
    Stack[Stack_Pointer] = m;
}

// Operations
void OPR(int m)
{
    if(m == 0) //RET
   {
       Stack_Pointer = Base_Pointer - 1;
       Program_Counter = Stack[Stack_Pointer + 4];
       Base_Pointer = Stack[Stack_Pointer + 3];
   }

    else if(m == 1) //NEG
        Stack[Stack_Pointer] = 0 - Stack[Stack_Pointer];

    else if(m == 2) //ADD
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] + Stack[Stack_Pointer + 1];
    }

    else if(m == 3) //SUB
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] - Stack[Stack_Pointer + 1];
    }

    else if(m == 4) //MUL
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] * Stack[Stack_Pointer + 1];
    }

    else if(m == 5) //DIV
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] / Stack[Stack_Pointer + 1];
    }

    else if(m == 6) //ODD
        Stack[Stack_Pointer] = Stack[Stack_Pointer] % 2;

    else if(m == 7) //MOD
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] % Stack[Stack_Pointer + 1];
    }

    else if(m == 8) //EQL
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] == Stack[Stack_Pointer + 1];
    }

    else if(m == 9) //NEQ
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] != Stack[Stack_Pointer + 1];
    }

    else if(m == 10) //LSS
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] < Stack[Stack_Pointer + 1];
    }

    else if(m == 11) //LEQ
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] <= Stack[Stack_Pointer + 1];
    }

    else if(m == 12) //GTR
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] > Stack[Stack_Pointer + 1];
    }

    else if(m == 13) //GEQ
    {
        Stack_Pointer--;
        Stack[Stack_Pointer] = Stack[Stack_Pointer] >= Stack[Stack_Pointer + 1];
    }
}

// Load
void LOD(int l, int m)
{
    Stack_Pointer++;
    Stack[Stack_Pointer] = Stack[ base(l, Base_Pointer) + m];
}

// Store
void STO(int l, int m)
{
    Stack[ base(l, Base_Pointer) + m] = Stack[ Stack_Pointer ];
    Stack_Pointer--;
}

// Call Subroutine
void CAL(int l, int m)
{
    Stack[Stack_Pointer + 1] = 0;
    Stack[Stack_Pointer + 2] = base(l, Base_Pointer);
    Stack[Stack_Pointer + 3] = Base_Pointer;
    Stack[Stack_Pointer + 4] = Program_Counter;
    Base_Pointer = Stack_Pointer + 1;
    Program_Counter = m;
}

// Increment
void INC(int m)
{
    Stack_Pointer += m;
}

// Jump
void JMP(int m)
{
    Program_Counter = m;
}

// Conditional Jump
void JPC(int m)
{
    if ( Stack[ Stack_Pointer ] == 0 )
        Program_Counter = m;

    Stack_Pointer--;
}

// String Input/Output
void SIO(int m)
{
    if(m == 0)
    {
        printf("\n\t%d ",Stack[ Stack_Pointer ]);
        Stack_Pointer--;
    }

    if (m == 1)
    {
        Stack_Pointer++;
        scanf("%d", &Stack[ Stack_Pointer ]);
    }

}
