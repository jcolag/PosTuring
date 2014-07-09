#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/***
 *** Data Structures
 ***/

struct tape_pos {
    char                symbol;
    struct tape_pos    *left,
                       *right;
}                      *tape;

/***
 *** Global Variables
 ***/

/* The Memory of the machine, the instruction pointer, and the symbol table */
int CodeSpace[1024][2],
    label[256];

/***
 *** Function Prototypes
 ***/

/* Utility functions */

int parse_post_turing(FILE *, FILE *, int[][2], int *);
void interpret_post_turing(int, int [][2]);
int write_to_tape(char *);
void display_tape(int);
void destroy_tape(void);

/* Language instructions */

void Right(void);
void Left(void);
void Print(char);
void Cond(char, char, int *);

/***
 *** Main program
 ***/

int main(int argc, char *argv[]) {
    FILE  *infile;
    char   initial[128];
    int    currinst = 0, idx;

    /*
     * Program Setup
     */
    tape = malloc(sizeof(struct tape_pos));
    tape->right = tape->left = NULL;
    tape->symbol = '0';

    for(idx = 0; idx < 256; idx++) {
        label[idx] = -1;
    }

    if(argc < 2) {
        fprintf(
          stderr,
          "\nUsage:  %s <filename>\n\tWhere <filename> a valid Post-Turing program.\n\n",
          argv[0]);
        exit(0);
    }

    infile = fopen(argv[1], "r");
    if(infile == NULL) {
        fprintf(stderr, "\nFile \"%s\" does not exist.\n\n", argv[1]);
        exit(1);
    }

    /*
     * Parse and run.
     */
    currinst = parse_post_turing(infile, stderr, CodeSpace, label);

    printf("Enter initial conditions:\n");
    scanf("%127[^\n\t]", initial);
    idx = write_to_tape(initial);

    for( ; idx >= 0; idx--) {
        Left();
    }

    interpret_post_turing(currinst, CodeSpace);

    fclose(infile);
    destroy_tape();
    return 0;
}
/*
 * Core runtime engine.  Checks type of statement and executes
 * appropriate function.
 */
void interpret_post_turing(int line, int code[1024][2]) {
    int instruction;

    display_tape(20);
    for(instruction = 0; instruction < line; instruction++) {
        switch(code[instruction][0]) {
        case 0:
            Right();
            break;
        case 1:
            Left();
            break;
        case 2:
            Print((char)code[instruction][1]);
            break;
        default:                        /* It's a label reference */
            Cond((char)(code[instruction][0] - 2), code[instruction][1], &instruction);
            break;
        }

        display_tape(20);
    }
}

/*
 * Straightforward parser for a very simple language.
 * Only checks the first character of keywords to identify the
 *   statements and ignores keywords that are only for structure.
 * Leaves a code identifying the statement type and parameter if any.
 */
int parse_post_turing(FILE *file, FILE *errfile, int code[1024][2], int *labels) {
    char    token[16];
    int     line = 0;

    while(fscanf(file, "%15s", token) > 0) {
        switch(tolower(token[0])) {
        case '[':                       /* Label */
            labels[token[1]] = line;        /* Label points to current line. */
            break;
        case 'i':                       /* If */
            fscanf(file, "%15s", token);
            code[line][0] = token[0] + 2;   /* Condition */
            fscanf(file, "%15s", token);    /* Goto... */
            fscanf(file, "%15s", token);
            code[line++][1] = token[0];     /* ...the label. */
            break;
        case 'l':                       /* Left */
            code[line++][0] = 1;
            break;
        case 'p':                       /* Print */
            code[line][0] = 2;
            fscanf(file, "%15s", token);
            code[line++][1] = token[0];
            break;
        case 'r':                       /* Right */
            code[line++][0] = 0;
            break;
        default:                        /* Other */
            fprintf(errfile, "Invalid program!!\n\n");
            exit(2);
            break;
        }
    }

    return line;
}

/*
 * write_to_tape() copies a string character-by-character onto the tape.
 *   Returns the number of characters copied from the string.
 */
int write_to_tape(char *string) {
    int     idx;
    char   *inputc;

    for(inputc = string, idx = 0; *inputc; inputc++, idx++) {
        Print(*inputc);
        Right();
    }

    return idx;
}

/*
 * display_tape() shows the local area of the tape around the head.
 */
void display_tape(int nLeft) {
    int         pos;
    static int  count = 0;

    for(pos = 0; pos < nLeft; pos++) {
        putchar(' ');
    }

    putchar('v');
    putchar('\n');

    for(pos = 0; pos < nLeft; pos++) {
        Left();
    }
    for(pos = 0; pos < 79; pos++) {
        putchar(tape->symbol);
        Right();
    }
    for(pos = 0; pos <(79 - nLeft); pos++) {
        Left();
    }

    putchar('\n');
    ++count;
}

/*
 * Destroy() wipes the dynamically-allocated tape cells.
 */
void destroy_tape(void)
{
    struct tape_pos    *head = tape;

    /* Move to the leftmost-allocated cell. */
    while (head->left) {
        head = head->left;
    }

    /* Head right, freeing each cell. */
    while (head) {
        struct tape_pos *shadow = head;
        head = head->right;
        free(shadow);
    }
}

/***
 *** Language instruction functions
 ***/

/*
 * Right() moves the tape head to the cell to the right.
 *   Note that the tape is dynamically allocated as needed.
 */
void Right(void) {
    if(tape->right == NULL) {
        tape->right = malloc(sizeof(struct tape_pos));
        tape->right->right = NULL;
        tape->right->left = tape;
        tape->right->symbol = '0';
    }

    tape = tape->right;
}

/*
 * Left()--surprise!--moves the tape head to the cell to the left.
 *   Note that the tape is dynamically allocated as needed.
 */
void Left(void) {
    if(tape->left == NULL) {
        tape->left = malloc(sizeof(struct tape_pos));
        tape->left->left = NULL;
        tape->left->right = tape;
        tape->left->symbol = '0';
    }

    tape = tape->left;
}

/*
 * Print() writes a symbol to the current tape cell.
 */
void Print(char Symbol) {
    tape->symbol = Symbol;
}

/*
 * Cond() compares the current cell's contents to the symbol,
 * and transfers control to the label if there's a match.
 */
void Cond(char Symbol, char newlabel, int *instruction) {
    if(tape->symbol == Symbol) {
        if(label[(int)newlabel] == -1) {
            printf("\nProgram Terminated\n");
            exit(0);
        } else {
            *instruction = label[(int)newlabel] - 1;
        }
    }
}
