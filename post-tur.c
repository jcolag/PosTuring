#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

struct  tape_pos
        {
         char                   symbol;
         struct tape_pos        *left, *right;
        }       *tape;

int     CodeSpace[1024][2], ip, label[256];
/* The Memory of the machine, the instruction pointer, and the symbol table */

void Right (void)
{
 if (tape->right == NULL)
        {
         tape->right = malloc (sizeof (struct tape_pos));
         tape->right->right = NULL;
         tape->right->left = tape;
         tape->right->symbol = '0';
        }
 tape = tape->right;
}

void Left (void)
{
 if (tape->left == NULL)
        {
         tape->left = malloc (sizeof (struct tape_pos));
         tape->left->left = NULL;
         tape->left->right = tape;
         tape->left->symbol = '0';
        }
 tape = tape->left;
}

void Print (char Symbol)
{
 tape->symbol = Symbol;
}

void IF (char Symbol, char newlabel)
{
 if (tape->symbol == Symbol)
        if (label[newlabel] != -1)
                ip = label[newlabel] - 1;
        else
                {
                 printf ("\nProgram Terminated\n");
                 exit (0);
                }
}

void Display (int nLeft)
{
 int    i;
 static int     count = 0;

 for (i = 0; i < nLeft; i++)
        putchar (' ');
 putchar ('v'); putchar ('\n');

 for (i = 0; i < nLeft; i++)
        Left ();
 for (i = 0; i < 79; i++)
        {
         putchar (tape->symbol);
         Right ();
        }
 for (i = 0; i < (79 - nLeft); i++)
        Left ();
 putchar ('\n');
 ++count;
}

int main (int argc, char *argv[])
{
 FILE   *infile;
 char   token[16], initial[128], *cp;
 int    currinst = 0, i;

 tape = malloc (sizeof (struct tape_pos));
 tape->right = tape->left = NULL;
 tape->symbol = '0';

 for (i = 0; i < 256; i++)
        label[i] = -1;

 if (argc < 2)
        {
         printf ("\nUsage:  %s <filename>\n\tWhere <filename>", argv[0]);
         printf (" a valid Post-Turing program.\n\n");
         exit (0);
        }

 infile = fopen (argv[1], "r");
 if (infile == NULL)
        {
         printf ("\nFile \"%s\" does not exist.\n\n", argv[1]);
         exit (1);
        }

 while (fscanf (infile, "%s", token) > 0)
        {
         switch (token[0] | 0x20)
                {
                 case 'r':
                        CodeSpace[currinst++][0] = 0;
                        break;
                 case 'l':
                        CodeSpace[currinst++][0] = 1;
                        break;
                 case ('[' | 0x20):
                        label[token[1]] = currinst;
                        break;
                 case 'i':
                        fscanf (infile, "%s", token);
                        CodeSpace[currinst][0] = token[0] + 2;
                        fscanf (infile, "%s", token);   /* Goto */
                        fscanf (infile, "%s", token);
                        CodeSpace[currinst++][1] = token[0];
                        break;
                 case 'p':
                        CodeSpace[currinst][0] = 2;
                        fscanf (infile, "%s", token);
                        CodeSpace[currinst++][1] = token[0];
                        break;
                 default:
                        fprintf (stderr, "Invalid program!!\n\n");
                        exit (2);
                }
        }

 i = 0;
 printf ("Enter initial conditions:\n");
 scanf ("%[^\n\t]", initial);
 for (cp = initial; *cp; cp++)
        {
         Print (*cp);
         Right ();
         ++i;
        }
 for ( ; i >= 0; i--)
        Left ();

 Display (20);
 for (ip = 0; ip < currinst; ip++)
        {
         switch (CodeSpace[ip][0])
                {
                 case 0:
                        Right ();
                        break;
                 case 1:
                        Left ();
                        break;
                 case 2:
                        Print ((char)CodeSpace[ip][1]);
                        break;
                 default:
                        IF ((char)(CodeSpace[ip][0] - 2), CodeSpace[ip][1]);
                        break;
                }
         Display (20);
        }
 
 printf ("Program Terminated\n");
 return 0;
}
