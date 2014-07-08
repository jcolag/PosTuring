#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

struct  tape_pos {
    char                symbol;
    struct tape_pos    *left,
                       *right;
}                      *tape;

/* The Memory of the machine, the instruction pointer, and the symbol table */
int     CodeSpace[1024][2],
        label[256];

/***
 *** Function Prototypes
 ***/

/* Utility functions */

int parse_post_turing (FILE *, FILE *, int[][2], int *);
void interpret_post_turing (int, int [][2]);

/* Language instructions */

void Right (void);
void Left (void);
void Print (char);
void IF (char, char, int *);
void Display (int);

/***
 *** Main program
 ***/

int main (int argc, char *argv[]) {
    FILE  *infile;
    char   initial[128], *inputc;
    int    currinst = 0, idx;

    tape = malloc (sizeof (struct tape_pos));
    tape->right = tape->left = NULL;
    tape->symbol = '0';

    for (idx = 0; idx < 256; idx++) {
        label[idx] = -1;
    }

    if (argc < 2) {
        printf (
          "\nUsage:  %s <filename>\n\tWhere <filename> a valid Post-Turing program.\n\n",
          argv[0]);
        exit (0);
    }

    infile = fopen (argv[1], "r");
    if (infile == NULL) {
        printf ("\nFile \"%s\" does not exist.\n\n", argv[1]);
        exit (1);
    }

    currinst = parse_post_turing(infile, stderr, CodeSpace, label);

    printf ("Enter initial conditions:\n");
    scanf ("%127[^\n\t]", initial);
    for (inputc = initial, idx = 0; *inputc; inputc++, idx++) {
        Print (*inputc);
        Right ();
    }

    for ( ; idx >= 0; idx--) {
        Left ();
    }

    interpret_post_turing (currinst, CodeSpace);

    printf ("Program Terminated\n");
    fclose (infile);
    return 0;
}

void interpret_post_turing (int line, int code[1024][2]) {
    int instruction;

    Display (20);
    for (instruction = 0; instruction < line; instruction++) {
        switch (code[instruction][0]) {
        case 0:
            Right ();
            break;
        case 1:
            Left ();
            break;
        case 2:
            Print ((char)code[instruction][1]);
            break;
        default:
            IF ((char)(code[instruction][0] - 2), code[instruction][1], &instruction);
            break;
        }

        Display (20);
    }
}

int parse_post_turing (FILE *file, FILE *errfile, int code[1024][2], int *labels) {
    char    token[16];
    int     line = 0;

    while (fscanf (file, "%15s", token) > 0) {
        switch (tolower(token[0])) {
        case 'r':                       /* Right */
            code[line++][0] = 0;
            break;
        case 'l':                       /* Left */
            code[line++][0] = 1;
            break;
        case '[':                       /* Label */
            labels[token[1]] = line;
            break;
        case 'i':                       /* If */
            fscanf (file, "%15s", token);
            code[line][0] = token[0] + 2;   /* Goto */
            fscanf (file, "%15s", token);
            fscanf (file, "%15s", token);
            code[line++][1] = token[0];
            break;
        case 'p':                       /* Print */
            code[line][0] = 2;
            fscanf (file, "%15s", token);
            code[line++][1] = token[0];
            break;
        default:                        /* Other */
            fprintf (errfile, "Invalid program!!\n\n");
            exit (2);
            break;
        }
    }

    return line;
}

/***
 *** Language instruction functions
 ***/

/*
 * Right() moves the tape head to the cell to the right.
 */
void Right (void) {
    if (tape->right == NULL) {
        tape->right = malloc (sizeof (struct tape_pos));
        tape->right->right = NULL;
        tape->right->left = tape;
        tape->right->symbol = '0';
    }

    tape = tape->right;
}

/*
 * Left()--surprise!--moves the tape head to the cell to the left.
 */
void Left (void) {
    if (tape->left == NULL) {
        tape->left = malloc (sizeof (struct tape_pos));
        tape->left->left = NULL;
        tape->left->right = tape;
        tape->left->symbol = '0';
    }

    tape = tape->left;
}

/*
 * Print() writes a symbol to the current tape cell.
 */
void Print (char Symbol) {
    tape->symbol = Symbol;
}

/*
 * IF() compares the current cell's contents to the symbol,
 * and transfers control to the label if there's a match.
 */
void IF (char Symbol, char newlabel, int *instruction) {
    if (tape->symbol == Symbol) {
        if (label[(int)newlabel] == -1) {
            printf ("\nProgram Terminated\n");
            exit (0);
        } else {
            *instruction = label[(int)newlabel] - 1;
        }
    }
}

/*
 * Display() shows the local area of the tape around the head.
 */
void Display (int nLeft) {
    int         pos;
    static int  count = 0;

    for (pos = 0; pos < nLeft; pos++) {
        putchar (' ');
    }

    putchar ('v');
    putchar ('\n');

    for (pos = 0; pos < nLeft; pos++) {
        Left ();
    }
    for (pos = 0; pos < 79; pos++) {
        putchar (tape->symbol);
        Right ();
    }
    for (pos = 0; pos < (79 - nLeft); pos++) {
        Left ();
    }

    putchar ('\n');
    ++count;
}
