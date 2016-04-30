//COP 3402 Summer 2015
//Program 3: Parser
//Jose Ocasio

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_SYMBOL_TABLE_SIZE 2000
#define MAX_CODE_LENGTH 2000
#define MAX_LEXI_LEVELS 3
#define IDLENGTH 12
#define NUMLENGTH 6

//enums
enum instruction_set_type
{
    Lit = 1, Opr, Lod, Sto, Cal, Inc, Jmp, Jpc, Sio
};
enum opr_type
{
    Ret, Neg, Add, Sub, Mul, Div, Odd, Mod, Eql, Neq, Lss, Leq, Gtr, Geq
};

typedef enum
{
	nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
	oddsym, eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym,
	ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym, elsesym, nlsym

} token_type;

//structs
typedef struct tokens
{
    int type;
    char name[IDLENGTH];

} tokened;

typedef struct instruction
{
	int OP;
	int L;
	int M;

} instruction;

typedef struct symbol
{
    int kind;          // const = 1, var = 2, proc = 3
    char name[12];     // name up to 11 chars
    int val;           // number (ASCII value)
    int level;         // L level
    int addr;          // M address

} symbol;

//function prototypes

void performOp();
int base(int level, int b);
void printStackTrace();
void printOutput(int flag);
void deleteAllComments();
void *lexScanner(int flag);
void printLexList(int flag);
void printLexTable();
void printFinalList(int flag);
void performVM(int flag);
void performParser(int flag);
void errorLog(int errorNum);
void prepareInput();
void generateStack();
void processStack(int flag, int canEX);
void runFC();
void runEC(int flag);
void retrieveTok();
void addToStack(int OP, int L, int M);
void emit(int kind, tokened token_type, int L, int M, int num);
int  cast(char *num);
int  searchSymbolTable(char *name, int level);
void finalize();
void dumpSymbols(int l);
void printAssembly();
void BLOCK();
void CONSTANT();
void CONDITION();
void PROGRAM();
void TERM();
void FACTOR();
void EXPRESSION();
void VARIABLE();
void PROCEDURE();
void STATEMENT();

//global vars
FILE* fp_t, *fp_c, *fp_cc, *fp_lt, *fp_ls, *fp_m;
const char* Operations[] = {"Default","Lit", "Opr", "Lod", "Sto", "Cal", "Inc", "Jmp", "Jpc", "Sio"};
const char* Stack_Ops[] = {"Ret", "Neg", "Add", "Sub", "Mul", "Div", "Odd", "Mod", "Eql", "Neq", "Lss", "Leq", "Gtr", "Geq"};
int basePointer = 1, stackPointer = 0, programCounter = 0;
int tPosition = 0, rs = 1, cs = 0;
int stack[MAX_STACK_HEIGHT];
int stackCodeSize = 0;
int symPos = 0, tokPos = 0;
int assemPos = 0, mPointer = 0, curLevel = 0, cn = 0, rn = 1;
int counter = 0, procedureCount = 0, levelVar = 0, levelConst = 0, varCount = 0, constCount = 0;
instruction instructionRegister;
instruction stackCode[MAX_CODE_LENGTH];
instruction assemblyCode[MAX_CODE_LENGTH];
tokened tokenArray[MAX_CODE_LENGTH];
tokened tokenPointer;
symbol symTable[MAX_SYMBOL_TABLE_SIZE];


int main(int argc, char *argv[])
{
    //l for lex, a for parser, v for VM
    int l = 0, a = 0, v = 0, i = 0;

    //compiler directives
    if(argc != 0)
    {
        for(i = 0; i < argc; i++)
        {
            if(strcmp(argv[i], "-l") == 0)
                l = 1;
            if(strcmp(argv[i], "-a") == 0)
                a = 1;
            if(strcmp(argv[i], "-v") == 0)
                v = 1;
        }
    }

    //perform based on directives
    lexScanner(l);
    performParser(a);
    performVM(v);

    return 0;
}


/*-----------------------------------------------------BEGIN LEXICAL ANALYZER----------------------------------------------------*/

//method to delete comments from pl/0 input file
void deleteAllComments()
{
    //variable init
    char numArray[NUMLENGTH];
    char varArray[IDLENGTH];
    int cnt = 0;

    char lexScanner;
    int flag = 0;
    int omitFlag = 0;

    //cleaned file
    fp_cc = fopen("cleaninput.txt","w");

    //check if file DNE
    if(fp_cc == NULL)
        errorLog(1);

    fscanf(fp_c, "%c", &lexScanner);


    //scan character by character through the input file and use
    //ASCII values to lookahead and decide if we have encountered a comment
    while (lexScanner != EOF)
    {
        if(flag == 1)
        {
            lexScanner = fgetc(fp_c);

            if(lexScanner == 10)
            {
                tokenArray[tPosition++].type = nlsym; rs++; cs = tPosition;
            }
            else if (lexScanner == '*')
            {
                lexScanner = fgetc(fp_c);
                if(lexScanner == '/')
                {
                    flag = 0;
                    lexScanner = fgetc(fp_c);
                }
            }
        }
        else
        {
            if((int)lexScanner == 32 || ((int)lexScanner == 9))
            {
                fprintf(fp_cc,"%c",lexScanner);
                lexScanner = fgetc(fp_c);
            }
            else if(((int)lexScanner == 10) || ((int)lexScanner == 59))
            {
                fprintf(fp_cc,"%c", lexScanner);

                //line feed
                if(lexScanner == 10)
                {
                    tokenArray[tPosition++].type = nlsym; rs++; cs = tPosition;
                }
                //semi-colon
                if(lexScanner == 59)
                {
                    strcpy(tokenArray[tPosition].name, ";");
                    tokenArray[tPosition++].type = semicolonsym;
                }

                lexScanner = fgetc(fp_c);

                //semi-colon, line feed, horizontal tab, end of text, or space
                while((((int)lexScanner == 59) || (int)lexScanner == 10) || ((int)lexScanner == 9) || ((int)lexScanner == 3) || ((int)lexScanner == 32))
                {
                    fprintf(fp_cc,"%c",lexScanner);

                    //semi-colon
                    if(lexScanner == 59)
                    {
                        strcpy(tokenArray[tPosition].name, ";");
                        tokenArray[tPosition++].type = semicolonsym;
                    }

                    //line feed
                    if(lexScanner == 10)
                    {
                        tokenArray[tPosition++].type = nlsym; rs++; cs = tPosition;
                    }
                    lexScanner = fgetc(fp_c);
                }
            }

            //represents a range of symbols in ASCII
            else if (((int)lexScanner >= 58 && (int)lexScanner <= 62) || ((int)lexScanner >= 40 && (int)lexScanner <= 47))
            {
                switch(lexScanner)
                {
                    case '+' :
                        strcpy(tokenArray[tPosition].name, "+");
                        tokenArray[tPosition++].type = plussym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case '-' :
                        strcpy(tokenArray[tPosition].name, "-");
                        tokenArray[tPosition++].type = minussym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case '*' :
                        strcpy(tokenArray[tPosition].name, "*");
                        tokenArray[tPosition++].type = multsym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    //this case is possibly the start of a comment, needs further investigation
                    case '/' :
                        lexScanner = fgetc(fp_c);

                        //there is a comment, set comment flag to 1
                        if(lexScanner == '*')
                            flag = 1;

                        else
                        {
                            strcpy(tokenArray[tPosition].name, "/");
                            tokenArray[tPosition++].type = slashsym;
                            fprintf(fp_cc,"/");
                        }

                        break;

                    case '=' :
                        strcpy(tokenArray[tPosition].name, "=");
                        tokenArray[tPosition++].type = eqlsym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case ',' :
                        strcpy(tokenArray[tPosition].name, ",");
                        tokenArray[tPosition++].type = commasym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case '.' :
                        strcpy(tokenArray[tPosition].name, ".");
                        tokenArray[tPosition++].type = periodsym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case '>' :
                        lexScanner = fgetc(fp_c);

                        if (lexScanner == '=')
                        {
                            strcpy(tokenArray[tPosition].name, ">=");
                            tokenArray[tPosition++].type = geqsym;
                            fprintf(fp_cc,">=");
                            lexScanner = fgetc(fp_c);
                        }
                        else
                        {
                            strcpy(tokenArray[tPosition].name, ">");
                            tokenArray[tPosition++].type = gtrsym;
                            fprintf(fp_cc,">");
                        }
                        break;

                    case '(' :
                        strcpy(tokenArray[tPosition].name, "(");
                        tokenArray[tPosition++].type = lparentsym;

                        fprintf(fp_cc,"(");
                        lexScanner = fgetc(fp_c);
                        break;

                    case ')' :
                        strcpy(tokenArray[tPosition].name, ")");
                        tokenArray[tPosition++].type = rparentsym;

                        fprintf(fp_cc,"%c", lexScanner);
                        lexScanner = fgetc(fp_c);
                        break;

                    case '<' :
                        lexScanner = fgetc(fp_c);
                        if (lexScanner == '=')
                        {
                            strcpy(tokenArray[tPosition].name, "<=");
                            tokenArray[tPosition++].type = leqsym;
                            fprintf(fp_cc,"<=");
                            lexScanner = fgetc(fp_c);
                        }
                        else if (lexScanner == '>')
                        {
                            strcpy(tokenArray[tPosition].name, "<>");
                            tokenArray[tPosition++].type = neqsym;
                            fprintf(fp_cc,"<>");
                            lexScanner = fgetc(fp_c);
                        }
                        else
                        {
                            strcpy(tokenArray[tPosition].name, "<");
                            tokenArray[tPosition++].type = lessym;
                            fprintf(fp_cc,"<");
                        }
                        break;

                    case ':' :
                        lexScanner = fgetc(fp_c);

                        if (lexScanner == '=')
                        {
                            strcpy(tokenArray[tPosition].name, ":=");
                            tokenArray[tPosition++].type = becomessym;
                            fprintf(fp_cc,":=");

                            lexScanner = fgetc(fp_c);
                        }
                        else
                        {
                            printf("\nError on line %d. ", rs, tPosition - cs + 1);
                            errorLog(18);
                        }
                        break;

                    default:
                        printf("\nError on line %d. ", rs, tPosition - cs + 1);
                        errorLog(18);
                }

                if(lexScanner == EOF)
                {
                    break;
                }
            }

            else if ((int)lexScanner <= 57 && (int)lexScanner >= 48)
            {
               numArray[0] = '\0';
               numArray[0] = lexScanner;
               cnt = 1;
               omitFlag = 0;

                while((int)lexScanner <= 57 && (int)lexScanner >= 48)
                {
                    //check if a num is too long
                    if(cnt >= NUMLENGTH)
                    {
                        printf("\nError on line %d. ", rs, tPosition - cs + 1);
                        errorLog(20);
                    }

                    lexScanner = fgetc(fp_c);

                    if(((int)lexScanner >= 58 && (int)lexScanner <= 62) || ((int)lexScanner >= 40 && (int)lexScanner <= 47)
                    || ((int)lexScanner == 32) || ((int)lexScanner == 10) || ((int)lexScanner == 9) || ((int)lexScanner == 59))
                    {
                        omitFlag = 1;
                    }

                    if(!omitFlag)
                    {
                        numArray[cnt++] = lexScanner;
                    }
                }

                numArray[cnt] = '\0';
                strcpy(tokenArray[tPosition].name,numArray);
                tokenArray[tPosition++].type = numbersym;
                fprintf(fp_cc,"%s", numArray);

                if(!omitFlag)
                {
                    lexScanner = fgetc(fp_c);
                }
            }

            else if ((int)lexScanner <= 122 && (int)lexScanner >= 97)
            {

                varArray[0] = '\0';
                varArray[0] = lexScanner;
                cnt = 1;
                omitFlag = 0;


                while((((int)lexScanner <= 57 && (int)lexScanner >= 48) || ((int)lexScanner <= 122 && (int)lexScanner >= 97 )) && (int)lexScanner > 32)
                {
                    //check if an identifier is too long
                    if(cnt >= IDLENGTH)
                    {
                        printf("\nError on line %d. ", rs, tPosition - cs + 1);
                        errorLog(21);
                    }

                    if((int)lexScanner < 32)
                    {
                        break;
                    }

                    lexScanner = fgetc(fp_c);

                    if(((int)lexScanner >= 58 && (int)lexScanner <= 62) || ((int)lexScanner >= 40 && (int)lexScanner <= 47) ||
                       ((int)lexScanner == 32) || ((int)lexScanner == 10) || ((int)lexScanner == 9) || ((int)lexScanner == 59))
                    {
                        omitFlag = 1;
                    }

                    if(!omitFlag)
                    {
                        varArray[cnt++] = lexScanner;
                    }
                }
                varArray[cnt] = '\0';

                if((int)varArray[cnt-1] < 32)
                {
                    varArray[cnt - 1] = '\0';
                }

                //setting syms
                if (strcmp(varArray, "begin") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "begin");
                    tokenArray[tPosition++].type = beginsym;
                }
                else if (strcmp(varArray, "call") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "call");
                    tokenArray[tPosition++].type = callsym;
                }
                else if (strcmp(varArray, "const") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "const");
                    tokenArray[tPosition++].type = constsym;
                }
                else if (strcmp(varArray, "var") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "var");
                    tokenArray[tPosition++].type = varsym;
                }
                else if (strcmp(varArray, "do") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "do");
                    tokenArray[tPosition++].type = dosym;
                }
                else if (strcmp(varArray, "else") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "else");
                    tokenArray[tPosition++].type = elsesym;
                }
                else if (strcmp(varArray, "end") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "end");
                    tokenArray[tPosition++].type = endsym;
                }
                else if (strcmp(varArray, "if") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "if");
                    tokenArray[tPosition++].type = ifsym;
                }
                else if (strcmp(varArray, "write") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "write");
                    tokenArray[tPosition++].type = writesym;
                }
                else if (strcmp(varArray, "procedure") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "procedure");
                    tokenArray[tPosition++].type = procsym;
                }
                else if (strcmp(varArray, "then") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "then");
                    tokenArray[tPosition++].type = thensym;
                }
                else if (strcmp(varArray, "while") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "while");
                    tokenArray[tPosition++].type = whilesym;
                }
                else if (strcmp(varArray, "read") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "read");
                    tokenArray[tPosition++].type = readsym;
                }
                else if (strcmp(varArray, "odd") == 0)
                {
                    strcpy(tokenArray[tPosition].name, "odd");
                    tokenArray[tPosition++].type = oddsym;
                }
                else
                {
                    strcpy(tokenArray[tPosition].name,varArray);
                    tokenArray[tPosition++].type = identsym;
                }

                fprintf(fp_cc,"%s", varArray);

                if(!omitFlag)
                {
                    lexScanner = fgetc(fp_c);
                }
            }
            else
            {
                if((int)lexScanner < 32)
                {
                    lexScanner = fgetc(fp_c);
                }
                else if(lexScanner == '/')
                {
                    lexScanner = fgetc(fp_c);

                    if(lexScanner == '*')
                    {
                        flag = 1;
                        lexScanner = fgetc(fp_c);
                    }
                    else
                    {
                        printf("\nError on line %d. ", rs, tPosition - cs + 1);
                        errorLog(18);
                    }
                }
                else
                {
                    printf("\nError on line %d. ", rs, tPosition - cs + 1);
                    errorLog(18);
                }
            }
        }
    }

    fclose(fp_cc);
}

//start of lexical process
void *lexScanner(int flag)
{
    fp_c = fopen("input.txt","r");

    if(fp_c == NULL)
        errorLog(1);

    deleteAllComments();

    printLexList(flag);

    fclose(fp_c);
}

//print lexeme table
void printLexTable()
{
    int i;

    fprintf(fp_lt,"lexeme      token type\n");

    for(i=0; i<tPosition; i++)
    {
        if(tokenArray[i].type == nlsym)
        {
        }
        else
        {
            fprintf(fp_lt,"%-11s %d\n", tokenArray[i].name, tokenArray[i].type);
        }
    }
}

//print lexeme list to screen
void printFinalList(int flag)
{
    int i;

    if(flag)
    {
        printf("\nlist of lexemes:\n");
    }

    for(i = 0; i < tPosition; i++)
    {
        if(tokenArray[i].type == nlsym)
        {
        }
        else
        {
            fprintf(fp_ls,"%d ", tokenArray[i].type);

            if(flag)
            {
                printf("%d ", tokenArray[i].type);
            }
            if(tokenArray[i].type == identsym || tokenArray[i].type == numbersym)
            {
                fprintf(fp_ls,"%s ", tokenArray[i].name);

                if(flag)
                {
                    printf("%s ", tokenArray[i].name);
                }
            }
        }
    }

    if(flag)
    {
        printf("\n");
    }
}

//print lexeme list to file
void printLexList(int flag)
{
    fp_lt = fopen("lexemetable.txt","w");

    if(fp_lt == NULL)
    {
        errorLog(23);
    }

    printLexTable();

    fclose(fp_lt);

    fp_ls = fopen("lexemelist.txt","w");

    if(fp_ls == NULL)
    {
        errorLog(23);
    }

    printFinalList(flag);

    fclose(fp_ls);
}

/*-----------------------------------------------------END LEXICAL ANALYZER----------------------------------------------------*/







/*-----------------------------------------------------BEGIN ASSEMBLY GENERATOR----------------------------------------------------*/

//parser error handling
void errorLog(int errorNum)
{
    switch(errorNum)
    {
        case 1:
            printf("The input file was invalid\n");
            break;
        case 2:
            printf("equals operator expected, become operator given\n");
            break;
        case 3:
            printf("become operator expected, equals operator given\n");
            break;
        case 4:
            printf("= was expected after constant declaration\n");
            break;
        case 5:
            printf("Constant equals must be followed by a number\n");
            break;
        case 6:
            printf("if must be followed by then\n");
            break;
        case 7:
            printf("while must be followed by do\n");
            break;
        case 8:
            printf("const, var, and procedure must be followed by an identifier\n");
            break;
        case 9:
            printf("identifier must be followed by become symbol in this case\n");
            break;
        case 10:
            printf("call must be followed by an identifier\n");
            break;
        case 11:
            printf("Relational operator expected\n");
            break;
        case 12:
            printf("Cannot assign to constant or procedure\n");
            break;
        case 13:
            printf("Semicolon or comma between statements missing\n");
            break;
        case 14:
            printf("STATEMENT began with an invalid symbol\n");
            break;
        case 15:
            printf("variable used without being declared first\n");
            break;
        case 16:
            printf(") is missing\n");
            break;
        case 17:
            printf("Invalid operator\n");
            break;
        case 18:
            printf("Invalid symbol\n");
            break;
        case 19:
            printf("semicolon expected\n");
            break;
        case 20:
            printf("a number was too long\n");
            break;
        case 21:
            printf("an identifier was too long\n");
            break;
        case 22:
            printf("stackCode length invalid\n");
            break;
        case 23:
            printf("Stack overflow (memory leak)\n");
            break;
        case 24:
            printf("Period expected\n");
            break;
        case 25:
            printf("duplicate var or const detected\n");
            break;
        default:
            printf("An unspecified error has occurred.\n");
        }

    //don't continue since we have an error
    exit(-1);
}

//print m code output
void printAssembly(int flag)
{
    char c;
    fp_m = fopen("mcode.txt","r");

    //cant open file
    if(fp_m == NULL)
    {
        errorLog(1);
    }

    if(flag)
    {
        printf("m code:\n");

        c = fgetc(fp_m);

        while(c != EOF)
        {
            printf("%c",c);
            c = fgetc(fp_m);
        }

        printf("\n\n");
    }

    fclose(fp_m);
}

//retrieve next token from token Array
void fetchToken()
{
    tokenPointer = tokenArray[tokPos];
    tokPos++;

    if(tokenPointer.type == nlsym)
    {
        while(tokenPointer.type == nlsym)
        {
            rn++;
            cn = 0;
            fetchToken();
        }
    }
    else
    {
        cn++;
    }
}

//open m code file and parse it, see if there are grammar errors
void performParser(int flag)
{
    fp_m = fopen("mcode.txt","w");

    if(fp_m == NULL)
    {
        errorLog(1);
    }

    //begin parsing process
    PROGRAM();

    printf("\nNo errors, program is syntactically correct.\n\n");

    finalize();

    fclose(fp_m);
    printAssembly(flag);
}

//go into block
void PROGRAM()
{
    fetchToken();

    BLOCK();

    //program should end with a period
    if(tokenPointer.type != periodsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(24);
    }
}

//main parsing block
void BLOCK()
{
    int i;
    int tassemPos = assemPos, temp;
    mPointer = 0;
    counter = 0;

    addToStack(7,0,0);

    while(tokenPointer.type == constsym || tokenPointer.type == varsym)
    {
        //if const detected, symbolize that
        if(tokenPointer.type == constsym)
        {
            CONSTANT();
        }
        //if variable detected, symbolize that
        if(tokenPointer.type == varsym)
        {
            VARIABLE();
        }

        counter++;
    }

    temp = mPointer;

    //if procedure detected, symbolize that
    while(tokenPointer.type == procsym)
        PROCEDURE();

    assemblyCode[tassemPos].M = assemPos;

    addToStack(6,0,temp + 4);
    STATEMENT();

    //if block not over
    if(tokenPointer.type != periodsym && tokenPointer.type == semicolonsym)
    {
        addToStack(2,0,0);
    }
    else
    {
        addToStack(9,0,2);
    }
}

void CONSTANT()
{
    tokened tempT;

    if(constCount >= 1)
    {
        if(levelConst == curLevel)
        {
            printf("\nError on line %d. ", rn);
            errorLog(25);
        }
    }

    constCount++;
    levelConst = curLevel;

    //check order of grammar
    do
    {
        fetchToken();

        if(tokenPointer.type != identsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(8);
        }

        strcpy(tempT.name, tokenPointer.name);

        fetchToken();

        if(tokenPointer.type != eqlsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(4);
        }

        fetchToken();

        if(tokenPointer.type != numbersym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(5);
        }

        emit(1, tempT, curLevel, -5, cast(tokenPointer.name));

        fetchToken();

    } while(tokenPointer.type == commasym);

    //make sure it ends with a semicolon
    if(tokenPointer.type != semicolonsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(13);
    }

    fetchToken();
}

void PROCEDURE()
{
    procedureCount++;
    fetchToken();

    //procedure must be followed by identifier
    if(tokenPointer.type != identsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(8);
    }

    //adding to sym table
    if(procedureCount == 1)
    {
        emit(3, tokenPointer, curLevel, assemPos, -1);
    }
    else
    {
        emit(3, tokenPointer, curLevel, assemPos, -1);
    }

    curLevel++;
    procedureCount++;
    varCount = 0;

    fetchToken();

    //end with ;
    if(tokenPointer.type != semicolonsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(13);
    }

    fetchToken();
    //back to block where we came from
    BLOCK();
    curLevel--;

    if(tokenPointer.type != semicolonsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(13);
    }

    fetchToken();
}

void VARIABLE()
{
    int run = 1;

    if(varCount >= 1)
    {
        if(levelVar == curLevel)
        {
            printf("\nError on line %d. ", rn);
            errorLog(25);
        }
    }

    varCount++;
    levelVar = curLevel;

    do
    {
        fetchToken();

        if(tokenPointer.type != identsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(8);
        }

        //add to sym table
        emit(2, tokenPointer, curLevel, mPointer+4, 0);

        fetchToken();

    } while(tokenPointer.type == commasym);

    if(tokenPointer.type != semicolonsym)
    {
        printf("\nError on line %d. ", rn);
        errorLog(13);
    }

    fetchToken();
}

void STATEMENT()
{
    int symPointer, idPointer;
    int tempBpPointer, tempPointer, temp2Pointer;

    if(tokenPointer.type == identsym)
    {
        symPointer = searchSymbolTable(tokenPointer.name, curLevel);

        if(symPointer == -1)
        {
            //error handling
            printf("\nError on line %d. ", rn);
            errorLog(15);
        }
        else if(symTable[symPointer].kind == 1)
        {
            printf("\nError on line %d. ", rn);
            errorLog(12);
        }

        idPointer = symTable[symPointer].addr;

        fetchToken();
        if(tokenPointer.type != becomessym)
        {
            if(tokenPointer.type == eqlsym)
            {
                printf("\nError on line %d. ", rn);
                errorLog(3);
            }
            else
            {
                printf("\nError on line %d. ", rn);
                errorLog(9);
            }
        }

        fetchToken();
        EXPRESSION();

        if(tokenPointer.type != semicolonsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(13);
        }

        addToStack(4, curLevel-symTable[symPointer].level, idPointer);
    }
    else if(tokenPointer.type == callsym)
    {
        fetchToken();

        if(tokenPointer.type != identsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(10);
        }

        symPointer = searchSymbolTable(tokenPointer.name, curLevel);

        if(symPointer == -1)
        {
            printf("\nError on line %d. ", rn);
            errorLog(15);
        }
        else if(symTable[symPointer].kind == 1)
        {
            printf("\nError on line %d. ", rn);
            errorLog(12);
        }

        fetchToken();

        addToStack(5, curLevel, symTable[symPointer].addr);

    }
    else if(tokenPointer.type == beginsym)
    {
        fetchToken();
        STATEMENT();

        while(tokenPointer.type == semicolonsym)
        {
            fetchToken();
            STATEMENT();
        }

        if(tokenPointer.type != endsym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(14);
        }

        fetchToken();

        curLevel++;
        dumpSymbols(curLevel);
        curLevel--;
    }
    else if(tokenPointer.type == ifsym)
    {
        fetchToken();
        //go into condition
        CONDITION();

        if(tokenPointer.type != thensym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(6);
        }

        fetchToken();
        tempBpPointer = assemPos;

        addToStack(8, 0, 0);

        //back to statement
        STATEMENT();
        assemblyCode[tempBpPointer].M = assemPos;

        fetchToken();

        if(tokenPointer.type != elsesym)
        {
            tokPos--;
            tokPos--;

            tokenPointer.type = tokenArray[tokPos].type;
            strcpy(tokenPointer.name,tokenArray[tokPos].name);

            while(tokenPointer.type == nlsym)
            {
                tokPos--;
                tokenPointer.type = tokenArray[tokPos].type;
                strcpy(tokenPointer.name,tokenArray[tokPos].name);
            }
            cn--;
        }

        if(tokenPointer.type == elsesym)
        {
            assemblyCode[tempBpPointer].M = assemPos+1;

            tempBpPointer = assemPos;

            addToStack(7, 0, 0);

            fetchToken();
            //go into statement
            STATEMENT();
            assemblyCode[tempBpPointer].M = assemPos;
        }
    }
    //while
    else if(tokenPointer.type == whilesym)
    {
        tempPointer = assemPos;

        fetchToken();
        CONDITION();

        temp2Pointer = assemPos;

        addToStack(8, 0, 0);

        if(tokenPointer.type != dosym)
        {
            printf("\nError on line %d. ", rn);
            errorLog(7);
        }

        fetchToken();
        STATEMENT();

        addToStack(7, 0, tempPointer);

        assemblyCode[temp2Pointer].M = assemPos;
    }
    //read
    else if(tokenPointer.type == readsym)
    {
        fetchToken();

        if(tokenPointer.type == identsym)
        {
            //search symbol table for previous occurs
            symPointer = searchSymbolTable(tokenPointer.name, curLevel);

            if(symPointer == -1)
            {
                printf("\nError on line %d. ", rn);
                errorLog(15);
            }

            fetchToken();

            addToStack(9, 0, 1);

            addToStack(4, 0, symTable[symPointer].addr);
        }
    }
    //write
    else if(tokenPointer.type == writesym)
    {
        fetchToken();

        if(tokenPointer.type == identsym)
        {
            symPointer = searchSymbolTable(tokenPointer.name, curLevel);
            if(symPointer == -1)
            {
                printf("\nError on line %d. ", rn);
                errorLog(15);
            }

            fetchToken();

            addToStack(3, 0, symTable[symPointer].addr);

            addToStack(9, 0, 0);
        }
        else
        {
            printf("\nError on line %d. ", rn);
            errorLog(10);
        }
    }
}

//Term
void TERM()
{
    int opTemp;

    //go into factor
    FACTOR();

    while(tokenPointer.type == multsym || tokenPointer.type == slashsym)
    {
        opTemp = tokenPointer.type;
        fetchToken();

        //go into factor
        FACTOR();

        if(opTemp == multsym)
        {
            addToStack(2, 0, 4);
        }
        else
        {
            addToStack(2, 0, 5);
        }
    }
}

//Expression
void EXPRESSION()
{
    int opTemp;

    if(tokenPointer.type == plussym || tokenPointer.type == minussym)
    {
        opTemp = tokenPointer.type;

        if(opTemp == minussym)
        {
            addToStack(2, 0, 1);
        }
    }

    //go into term
    TERM();

    //if we are adding or subtracting
    while(tokenPointer.type == plussym || tokenPointer.type == minussym)
    {
        opTemp = tokenPointer.type;
        fetchToken();

        //go into term
        TERM();

        if(opTemp == plussym)
        {
            addToStack(2, 0, 2);
        }
        else
        {
            addToStack(2, 0, 3);
        }
    }
}

//factor
void FACTOR()
{
    int symPos;

    if(tokenPointer.type == identsym)
    {
        //search for previous occurs
        symPos = searchSymbolTable(tokenPointer.name, curLevel);

        if(symPos == -1)
        {
            printf("\nError on line %d. ", rn);
            errorLog(15);
        }

        if(symTable[symPos].kind == 1)
        {
            addToStack(1, 0, symTable[symPos].val);
        }
        else
        {
            addToStack(3, curLevel-symTable[symPos].level, symTable[symPos].addr);
        }

        fetchToken();
    }
    else if(tokenPointer.type == numbersym)
    {
        addToStack(1, 0, cast(tokenPointer.name));
        fetchToken();
    }
    else if (tokenPointer.type == lparentsym)
    {
        fetchToken();
        EXPRESSION();

        if(tokenPointer.type != rparentsym)
        {
            //expected )
            printf("\nError on line %d. ", rn);
            errorLog(16);
        }

        fetchToken();
    }
    else
    {
        //expected (
        printf("\nError on line %d. ", rn);
        errorLog(14);
    }
}

//condition
void CONDITION()
{
    int opTemp;

    if(tokenPointer.type == oddsym)
    {
        addToStack(2,0,6);
        fetchToken();

        EXPRESSION();
    }
    else
    {
        EXPRESSION();
        opTemp = tokenPointer.type;

        switch (tokenPointer.type)
        {
            case becomessym:
                printf("\nError on line %d. ", rn);
                errorLog(2);
                break;

            case eqlsym:
                opTemp = 8;
                break;

            case neqsym:
                opTemp = 9;
                break;

            case lessym:
                opTemp = 10;
                break;

            case leqsym:
                opTemp = 11;
                break;

            case gtrsym:
                opTemp = 12;
                break;

            case geqsym:
                opTemp = 13;
                break;

            default:
                printf("\nError on line %d. ", rn);
                errorLog(11);
                break;
        }

        fetchToken();
        EXPRESSION();
        addToStack(2, 0, opTemp);
    }
}

//method to build the stacktrace
void addToStack(int OP, int L, int M)
{
    assemblyCode[assemPos].OP = OP;
    assemblyCode[assemPos].M = M;
    assemblyCode[assemPos].L = L;

    assemPos++;
}

//method to search the symbol table
int searchSymbolTable(char *name, int level)
{
    int i;

    while(level != -1)
    {
        for(i = symPos - 1; i >= 0; i--)
        {
            if(strcmp(name,symTable[i].name) == 0 && symTable[i].addr != -1 && symTable[i].level == level)
            {
                return i;
            }
        }

        level--;
    }

    return -1;
}

//method to build the symbol table
void emit(int kind, tokened t, int L, int M, int num)
{
    symTable[symPos].kind = kind;
    strcpy(symTable[symPos].name,t.name);
    symTable[symPos].level = L;
    symTable[symPos].addr = M;

    if(kind == 1)
    {
        symTable[symPos].val = num;
    }
    else if (kind == 2)
    {
        mPointer++;
    }

    symPos++;
}

//cleanup
void dumpSymbols(int level)
{
    int i;

    for(i = symPos - 1; i >= 0; i--)
    {
        if(symTable[i].level == level && symTable[i].kind != 3)
        {
            symTable[i].addr = -1;
        }
    }
}

//casting ch to int datatype
int cast(char *num)
{
    int result = 0, i = 0;

    while(num[i] != '\0')
    {
        result *= 10;
        result += num[i] - '0';

        i++;
    }

    return result;
}

//print m code to file
void finalize()
{
    int i;

    for(i=0; i< assemPos; i++)
    {
        fprintf(fp_m,"%d %d %d\n",assemblyCode[i].OP, assemblyCode[i].L, assemblyCode[i].M);
    }
}


/*-----------------------------------------------------END ASSEMBLY GENERATOR----------------------------------------------------*/







/*----------------------------------------------------BEGIN STACK TRACE---------------------------------------------------*/

void performVM(int flag)
{
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = 0;

    fp_c = fopen("mcode.txt","r");

    if(fp_c == NULL)
    {
        errorLog(1);
    }

    prepareInput();

    fp_t = fopen("stacktrace.txt","w");

    if(fp_t == NULL)
    {
        errorLog(1);
    }

    generateStack(0);

    processStack(0, flag);

    fclose(fp_t);
    fclose(fp_c);
}

void generateStack(int flag)
{
	int i;

	fprintf(fp_t,"Line  OP   L  M\n");

	for(i = 0; i < stackCodeSize; i++)
    {
		fprintf(fp_t,"%4d  %s  %d  %2d\n", i, Operations[stackCode[i].OP], stackCode[i].L, stackCode[i].M);
	}

	if(flag)
    {
        printf("\nStack trace:\n                   pc  bp  sp  stack\nInitial values     %2d   %d  %2d\n", programCounter, basePointer, stackPointer);
    }

	fprintf(fp_t,"\n\n");
	fprintf(fp_t,"                   pc  bp  sp  stack\nInitial values     %2d   %d  %2d\n", programCounter, basePointer, stackPointer);
}

void prepareInput()
{
    int OP, L, M, i = 0;

    while(fscanf(fp_c,"%d",&OP) != EOF)
    {
        if(i>MAX_CODE_LENGTH)
        {
            errorLog(22);
        }

        fscanf(fp_c, "%d", &L);
        fscanf(fp_c, "%d", &M);

        stackCode[i].OP = OP;
        stackCode[i].L = L;
        stackCode[i].M = M;

        i++;
    }

    stackCodeSize = i;
}

void processStack(int flag, int canEX)
{
    while (basePointer > 0)
    {
        if (programCounter < stackCodeSize)
        {
            if(flag)
            {
                printf("%4d  %s  %d  %2d ", programCounter, Operations[stackCode[programCounter].OP], stackCode[programCounter].L, stackCode[programCounter].M);
            }

            fprintf(fp_t,"%4d  %s  %d  %2d ", programCounter, Operations[stackCode[programCounter].OP], stackCode[programCounter].L, stackCode[programCounter].M);

            runFC();
            runEC(canEX);

            if(flag)
            {
                printf("  %2d   %d  %2d  ", programCounter, basePointer, stackPointer);
            }

            fprintf(fp_t,"  %2d   %d  %2d  ", programCounter, basePointer, stackPointer);
            printStackTrace(flag);

            if(flag)
            {
                printf("\n");
            }

            fprintf(fp_t,"\n");
        }
        else
        {
            printOutput(canEX);
            exit(0);
        }
    }
}

void runEC(int flag)
{
    int val = 0;

	switch (instructionRegister.OP)
	{
		case Lit:
			stackPointer++;
			stack[stackPointer] = instructionRegister.M;
			break;

		case Opr:
			performOp();
			break;

		case Lod:
			stackPointer++;
			stack[stackPointer] = stack[base(instructionRegister.L, basePointer) + instructionRegister.M];
			break;

		case Sto:
			stack[base(instructionRegister.L, basePointer) + instructionRegister.M] = stack[stackPointer];
			stackPointer--;
			break;

		case Cal:
		    stack[stackPointer + 1] = 0;
            stack[stackPointer + 2] = base(instructionRegister.L, basePointer);
            stack[stackPointer + 3] = basePointer;
            stack[stackPointer + 4] = programCounter;
            basePointer = stackPointer + 1;
            programCounter = instructionRegister.M;
			break;

		case Inc:
			stackPointer += instructionRegister.M;
			break;

		case Jmp:
			programCounter = instructionRegister.M;
			break;

		case Jpc:
			if(stack[stackPointer] == 0)
            {
				programCounter = instructionRegister.M;
                stackPointer--;
			}
			else
			{
                stackPointer--;
			}
			break;

		case Sio:
			if(instructionRegister.M == 0)
            {
                printf("output: %d\n", stack[stackPointer]);
                stackPointer--;
			}
            else if(instructionRegister.M == 1)
            {
                printf("\nuser input: ");
                scanf("%d", &val);
                printf("\n");

                stackPointer++;
                stack[stackPointer] = val;
            }
            else if(instructionRegister.M == 2)
            {
                fclose(fp_t);
                printOutput(flag);
                exit(0);
            }
			break;

		case 10:
		    exit(0);
		    break;

		default:
			exit(-1);
    }
}

void runFC()
{
	instructionRegister = stackCode[programCounter];
	programCounter++;
}

void performOp()
{
	switch (instructionRegister.M)
	{
		case Ret:
			stackPointer = basePointer - 1;
			programCounter = stack[stackPointer + 4];
			basePointer = stack[stackPointer + 3];
			break;

		case Neg:
			stack[stackPointer] *= -1;
			break;

		case Add:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] + stack[stackPointer + 1];
			break;

		case Sub:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] - stack[stackPointer + 1];
			break;

		case Mul:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] * stack[stackPointer + 1];
			break;

		case Div:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] / stack[stackPointer + 1];
			break;

		case Odd:
			stack[stackPointer] %= 2;
			break;

		case Mod:
			stackPointer--;
			stack[stackPointer] %= stack[stackPointer + 1];
			break;

		case Eql:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] == stack[stackPointer+1];
			break;

		case Neq:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] != stack[stackPointer + 1];
			break;

		case Lss:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] < stack[stackPointer + 1];
			break;

		case Leq:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] <= stack[stackPointer + 1];
			break;

		case Gtr:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] > stack[stackPointer + 1];
			break;

		case Geq:
			stackPointer--;
			stack[stackPointer] = stack[stackPointer] >= stack[stackPointer + 1];
			break;

		default:
			errorLog(17);
	}
}

void printStackTrace(int flag)
{
	int bpCount = 1, i = 1, bpTemp = basePointer;
	int bpArray[MAX_LEXI_LEVELS];

	bpArray[0] = 1;

	while (bpTemp > 1)
    {
        bpArray[i++] = bpTemp;
        bpTemp = stack[bpTemp + 2];
	}

    bpCount = i - 1;

	for(i = 1; i <= stackPointer; i++)
    {
		if (i == bpArray[bpCount] && i != 1)
        {
            if(flag)
                printf("| ");
			fprintf(fp_t,"| ");

			bpCount--;
		}

		if(flag)
		{
            printf("%d ", stack[i]);
		}

		fprintf(fp_t,"%d ", stack[i]);
	}
}

int base (int level, int base)
{
	while(level > 0)
    {
		base = stack[base + 2];
		level--;
	}

	return base;
}

void printOutput(int flag)
{
    char tokenedArray[99], c;
    int run = 1;

    fp_t = fopen("stacktrace.txt", "r");

    if(flag)
    {
        run = strcmp("pc",tokenedArray);

        while(run != 0)
        {
            fscanf(fp_t,"%s",tokenedArray);
            run = strcmp("pc",tokenedArray);
        }

        printf("\n");
        fscanf(fp_t,"%s",tokenedArray);
        fscanf(fp_t,"%s",tokenedArray);
        fscanf(fp_t,"%s",tokenedArray);

        printf("                   pc  bp  sp  stack");

        while(c != EOF)
        {
            c = fgetc(fp_t);
            printf("%c", c);
        }

        printf("\n");
    }

    fclose(fp_t);
}


