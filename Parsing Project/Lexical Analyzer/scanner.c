/*
Jose Ocasio
COP 3402: Systems Software Summer 2015
Project Module #2: Lexical Analyzer
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define IDLENGTH  11
#define NUMLENGTH  5

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym } token_type;

FILE *fp_input, *fp_clean, *fp_table, *fp_list;

void PrintCleanInput(char *textarray);
void PrintLexeme(char *textarray);
int SkipComments(char *textarray, int resume);

int main()
{
    // File and Loop Variables
    int i = 0, c;
    char temp[10000];
    fp_input = fopen ("input.txt", "r");

    // File Not Found? Close Program
    if(fp_input == NULL)
    {
        fclose(fp_input);
        printf("File Not Found Or Does Not Exist. Program Terminated\n");
    }

    // Scan File Letter By Letter
    while( (c = fgetc(fp_input)) != EOF )
    {
        temp[i] = (char) c;
        i++;
    }

    PrintCleanInput(temp);
    PrintLexeme(temp);

    fclose(fp_input);
    return 0;
}

// Print Input File Without Comments
void PrintCleanInput(char *textarray)
{
    int i = 0;
    fp_clean = fopen ("cleaninput.txt", "w");

    // Loop Through Array
    for(i; textarray[i] != '\0'; i++)
    {
        // Skip Comments (47 = '/', 42 = '*')
        if((int) textarray[i] == 47 && (int) textarray[i+1] ==  42)
            i = SkipComments(textarray, i);

        // Print Char At i
        fprintf(fp_clean, "%c", textarray[i]);
    }

   fclose(fp_clean);
}

// Print Lexeme Table And List
void PrintLexeme(char *textarray)
{
    // i and j is for tokened[][] while k loops through text array
    int i = 0, j = 0, k = 0, value, tokentype[10000], prev_va = 0;
    char tokened[10000][IDLENGTH], ch;
    fp_table = fopen ("lexemetable.txt", "w");
    fp_list = fopen ("lexemelist.txt", "w");

    fprintf(fp_table, " Lexeme      Token Type\n");

    ch = textarray[k++];

    // Scan through array until the end and token each piece
    while(ch != '\0')
    {
        // Skip Comments
        if((int) ch == 47 && (int) textarray[k] ==  42)
        {
            k = SkipComments(textarray, k);
            ch = textarray[k++];
        }

        // nulsym
        if(ch == '\0')
            tokentype[i] = nulsym;

        // Words
        // Scan letter by letter and store result in token[i][j]. The word at token[i][] is a single token with token type of 2 or {21,33}
        if (isalpha(ch))
        {
            while ( (isalpha(ch) || isdigit(ch)))
            {
                tokened[i][j++] = ch;
                ch = textarray[k++];
            }

            if(strcmp(tokened[i], "odd") == 0)
                tokentype[i++] = oddsym;  // oddsym

            else if(strcmp(tokened[i], "begin") == 0)
                tokentype[i++] = beginsym;  // beginsym

            else if(strcmp(tokened[i], "end") == 0)
                tokentype[i++] = endsym;  // endsym

            else if(strcmp(tokened[i], "if") == 0)
                tokentype[i++] = ifsym;  // ifsym

            else if(strcmp(tokened[i], "then") == 0)
                tokentype[i++] = thensym;  // thensym

            else if(strcmp(tokened[i], "while") == 0)
                tokentype[i++] = whilesym;  // whilesym

            else if(strcmp(tokened[i], "do") == 0)
                tokentype[i++] = dosym;  // dosym

            else if(strcmp(tokened[i], "call") == 0)
                tokentype[i++] = callsym;  // callsym

            else if(strcmp(tokened[i], "const") == 0)
                tokentype[i++] = constsym;  // constsym

            else if(strcmp(tokened[i], "var") == 0)
                tokentype[i++] = varsym;  // varsym

            else if(strcmp(tokened[i], "procedure") == 0)
                tokentype[i++] = procsym;  // procsym

            else if(strcmp(tokened[i], "write") == 0)
                tokentype[i++] = writesym;  // writesym

            else if(strcmp(tokened[i], "read") == 0)
                tokentype[i++] = readsym;  // readsym

            else if(strcmp(tokened[i], "else") == 0)
                tokentype[i++] = elsesym;  // elsesym

            else
            {
                tokentype[i++] = identsym; // identsym
            }
            k--;
            j = 0;
        }




        // Numbers
        // Scan digit by digit and store result in token[i][j]. This result is a numbersym = 3
        else if (isdigit(ch))
        {
            while (isdigit(ch))
            {
                tokened[i][j++] = ch;
                ch = textarray[k++];
            }

            tokentype[i++] = numbersym;   // numbersym
            j = 0;
            k--;
        }




        // Symbols
        else if (ispunct(ch))
        {
            // Next: Is It A Symbol?
            if(ch == '+')
            {
                tokened[i][0] = ch;
                tokentype[i++] = plussym;   // plussym
            }

            else if(ch == '-')
            {
                tokened[i][0] = ch;
                tokentype[i++] = minussym;   // minussym
            }

           else if(ch == '*')
            {
                tokened[i][0] = ch;
                tokentype[i++] = multsym;   // multsym
            }

            else if(ch == '/')
            {
                tokened[i][0] = ch;
                tokentype[i++] = slashsym;   // slashsym
            }

            else if(ch == '=')
            {
                tokened[i][0] = ch;
                tokentype[i++] = eqsym;   // eqsym
            }

            else if(ch == '<')
            {
                ch = textarray[k++];
                if(ch == '=')
                    tokentype[i] = leqsym;      // leqsym

                else if(ch == '>')
                    tokentype[i] = neqsym;      // neqsym

                else
                    tokentype[i] = lessym;      // lessym

                tokened[i++][0] = ch;
            }

            else if(ch == '>')
            {
                ch = textarray[k++];
                if(tokened[i][1] == '=')
                    tokentype[i] = geqsym;      // geqsym

                else
                    tokentype[i] = gtrsym;      // gtrsym

                tokened[i++][0] = ch;
            }

            else if(ch == '(')
            {
                tokened[i][0] = ch;
                tokentype[i++] = lparentsym;  // lparentsym
            }

            else if(ch == ')')
            {
                tokened[i][0] = ch;
                tokentype[i++] = rparentsym;  // rparentsym
            }

            else if(ch == ',')
            {
                tokened[i][0] = ch;
                tokentype[i++] = commasym;  // commasym
            }

            else if(ch == ';')
            {
                tokened[i][0] = ch;
                tokentype[i++] = semicolonsym;  // semicolonsym
            }

            else if(ch == '.')
            {
                tokened[i][0] = ch;
                tokentype[i++] = periodsym;  // periodsym
            }

            else if(ch == ':')
            {
                if((ch = textarray[k++]) == '=')
                {
                    tokened[i][0] = ':';
                    tokened[i][1] = ch;
                    tokentype[i++] = becomessym;  // becomesym
                }
            }
            else
            {
                tokened[i][0] = ch;
                tokentype[i++] = nulsym;
            }
        }

        ch = textarray[k++];

    }

    // Print Each Token And Their Corresponding Type
    for(i = 0; tokentype[i] != '\0'; i++)
    {
        //Detect if a variable starts with a number
        if(isdigit(tokened[i][0]) && isalpha(tokened[i+1][0]))
        {
            fprintf(fp_table, "Error: A variable must start with a letter.");
            exit(-1);
        }

        //Detect if a variable name was too long
        if (tokentype[i] == identsym && strlen(tokened[i]) > IDLENGTH)
        {
            fprintf(fp_table, "Error: A name was too long.");
            exit(-1);
        }

        //Detect if a number was too long
        if (tokentype[i] == numbersym && strlen(tokened[i]) > NUMLENGTH)
        {
            fprintf(fp_table, "Error: A number was too long.");
            exit(-1);
        }

        if (tokentype[i] == nulsym)
        {
            fprintf(fp_table, "Error: Invalid Symbol detected.");
            exit(-1);
        }

        // Print To Lexeme Table And Lexeme List
        if (tokentype[i] != nulsym)
        {
            fprintf(fp_table, " %s\t\t%d\n", tokened[i], tokentype[i]);

            fprintf(fp_list,"%d ",  tokentype[i]);

            if(tokentype[i] == identsym) fprintf(fp_list, "%s ", tokened[i]);
        }
    }

    // Close Files
    fclose(fp_table);
    fclose(fp_list);
}


// Start Where Array Has /* And Return Location Of */
int SkipComments(char *textarray, int resume)
{
    int i = resume+2;

    while( (int) textarray[i] !=  42 && (int) textarray[i+1] !=  47)
        i++;

    return i+2;
}

