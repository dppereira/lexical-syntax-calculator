#include <lexer.h>
#include <parser.h>

int lookahead;

#include <string.h>

double acc;
double stack[STACKSIZE];
char symtab[SYMTABSIZE][MAXIDLEN + 1];
double varmem[SYMTABSIZE];
int symtab_next_entry = 0;
int sp = 0;

/*
LL(1) grammar:

E -> T RSYMTABSIZE
E -> [oplus] T [[]] R
oplus = '+' || '-'

T -> F Q
F -> (E) | OCT | DEC | HEX | ID

R -> +T R | -T R | <>
R -> { oplus T }
oplus = '+' || '-'

Q -> *F Q | /F Q | <>
Q -> { otimes F }
otimes = '*' || '/'
*/

void cmd(void) {
  switch (lookahead) {
  case ';':
  case '\n':
    match(lookahead);
    break;
  case EOF:
    match(EOF);
  case QUIT:
  case EXIT:
    exit(0);
  default:
    E();
    printf("\tConta: %lf\n", acc);
    break;
  }
}

void mybc(void) {
  while (1) {
    cmd();
  }
}

void push(double value) { stack[sp++] = value; }

double pop() { return stack[--sp]; }

int lookup(char *varname) {
  int i;

  for (i = 0; i < symtab_next_entry; i++) {
    if (strcmp(varname, symtab[i]) == 0)
      break;
  }

  if (i == symtab_next_entry) {
    strcpy(symtab[i], varname);
  }

  return i;
}

double recall(char *varname) {
  int i = lookup(varname);
  if (i != symtab_next_entry) {
    return varmem[i];
  }

  return varmem[0];
}

void store(char *varname, double value) {
  int i = lookup(varname);
  symtab_next_entry++;
  varmem[i] = value;
} 

void E(void) {

  /*0*/ int oplus = 0, signal = 0, otimes = 0; /*0*/

  if (lookahead == '+' || lookahead == '-') {
    signal = lookahead == '-' ? lookahead : 0;
    match(lookahead);
  }

  char varname[MAXIDLEN + 1];
_T:

_F:
  switch (lookahead) {
  case '(':
    match('(');
    E();
    match(')');
    break;
  case NUM:
    /**/ printf("\tmov %s, acc\n", lexeme); /**/
    acc = atof(lexeme);
    match(NUM);
    break;
  default:
    /*9*/ strcpy(varname, lexeme); /*9*/
    match(ID);
    if (lookahead == ASGN) {
      // L value variable
      match(ASGN);
      E();
      /**/ printf("\tstore  acc, %s\n", varname); /**/
      store(varname, acc);
    } else {
      // R value variable
      /**/ printf("\trecall  acc, %s\n", varname); /**/
      acc = recall(varname);
    }
  }

  printf("\tValor acc: %lf\n", acc);

  if (signal) {
    /*4*/ printf("\tneg acc\n"); /*4*/
    acc = -acc;
    signal = 0;
  }

  if (otimes) {
    /*1*/
    switch (otimes) {
    case '*':
      printf("\tmult stack[\'%c\'], acc\n", otimes);
      stack[sp - 1] *= acc;
      break;
    case '/':
      printf("\tdiv stack[\'%c\'], acc\n", otimes);
      stack[sp - 1] /= acc;
      break;
    }
    /*1*/
    printf("\tpop acc\n");
    acc = pop();
    otimes = 0;
  }

  if (lookahead == '*' || lookahead == '/') {
    /*1*/
    otimes = lookahead;
    printf("\tpush acc\n");
    match(lookahead);
    push(acc);
    /*1*/
    goto _F;
  }

  if (oplus) {
    /*5*/
    switch (oplus) {
    case '+':
      printf("\tadd acc, stack[\'%c\']\n", oplus);
      stack[sp - 1] += acc;
      break;
    case '-':
      printf("\tsub acc, stack[\'%c\']\n", oplus);
      stack[sp - 1] -= acc;
      break;
    }
    /*5*/
    printf("\tpop acc\n");
    acc = pop();
    oplus = 0;
  }

  if (lookahead == '+' || lookahead == '-') {
    /*3*/
    oplus = lookahead;
    printf("\tpush acc\n");
    push(acc);
    match(lookahead);
    /*3*/
    goto _F;
  }
}

void match(int expected) {
  if (lookahead == expected) {
    lookahead = gettoken(source);
  } else {
    fprintf(stderr, "token mismatch at line %d\n", linenum);
    exit(-3);
  }
}