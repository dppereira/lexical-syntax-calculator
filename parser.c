#include <lexer.h>
#include <parser.h>

int lookahead;

#include <string.h>

// Armazena o resultado das expressoes
double acc;

// Pilha utilizada para armazenamento de valores temporarios
double stack[STACKSIZE];

// Tabela de simbolos para armazenamento de nomes de variaveis
char symtab[SYMTABSIZE][MAXIDLEN + 1];

// Memoria para armazenamento de valores das variaveis
double varmem[SYMTABSIZE];

// Indice que aponta para a proxima entrada na tabela de simbolos
int symtab_next_entry = 0;

// Ponteiro de pilha
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
    printf("\tResultado: %lf\n", acc);
    break;
  }
}

// Cria o ambiente de execucao
void mybc(void) {
  while (1) {
    cmd();
  }
}

// Faz o push de um valor para a pilha
void push(double value) { stack[sp++] = value; }

// Faz o pop da pilha
double pop() { return stack[--sp]; }

// Procura um nome de variavel na tabela de simbolos
int lookup(char *varname) {
  int i;

  for (i = 0; i < symtab_next_entry; i++) {
    if (strcmp(varname, symtab[i]) == 0)
      break;
  }

  // Se a variavel nao foi encontrada, adiciona na tabela
  if (i == symtab_next_entry) {
    strcpy(symtab[i], varname);
  }

  return i;
}

// Recupera o valor de uma variavel
double recall(char *varname) {
  int i = lookup(varname);
  if (i != symtab_next_entry) {
    return varmem[i];
  }

  return varmem[0];
}

// Armazena o valor de uma variavel
void store(char *varname, double value) {
  int i = lookup(varname);
  symtab_next_entry++;
  varmem[i] = value;
}

void E(void) {

  /*0*/ int oplus = 0, signal = 0, otimes = 0; /*0*/

  // Ajusta o sinal caso token seja '+' ou '-'
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
    /*1*/ printf("\tmov %s, acc\n", lexeme); /*1*/
    acc = atof(lexeme);
    match(NUM);
    break;
  default:
    /*2*/ strcpy(varname, lexeme); /*2*/
    match(ID);
    // Se o token for ASGN processa a expressao
    if (lookahead == ASGN) {
      match(ASGN);
      E();
      /*3*/ printf("\tstore  acc, %s\n", varname); /*3*/
      store(varname, acc);
    } else {
      // Trata como variavel de leitura
      /*4*/ printf("\trecall  acc, %s\n", varname); /*4*/
      acc = recall(varname);
    }
  }

  // Se houver sinal negativo, inverte o sinal do acumulador
  if (signal) {
    /*5*/ printf("\tneg acc\n"); /*5*/
    acc = -acc;
    signal = 0;
  }

  // Se houver operacao de multiplicacao ou divisao faz operacao com o valor do topo da pilha
  if (otimes) {
    /*6*/
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
    /*6*/
    printf("\tpop acc\n");
    acc = pop();
    otimes = 0;
  }

  // Armazena o resultado da operacao em acc
  if (lookahead == '*' || lookahead == '/') {
    /*7*/
    otimes = lookahead;
    printf("\tpush acc\n");
    match(lookahead);
    push(acc);
    /*7*/
    goto _F;
  }

  // Se houver operacao de adicao ou subtracao faz operacao com o valor do topo da pilha
  if (oplus) {
    /*8*/
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
    /*8*/
    printf("\tpop acc\n");
    acc = pop();
    oplus = 0;
  }

  // Armazena o resultado da operacao em acc
  if (lookahead == '+' || lookahead == '-') {
    /*9*/
    oplus = lookahead;
    printf("\tpush acc\n");
    push(acc);
    match(lookahead);
    /*9*/
    goto _F;
  }
}

// Verifica se o token atual corresponde ao esperado
void match(int expected) {
  if (lookahead == expected) {
    lookahead = gettoken(source);
  } else {
    fprintf(stderr, "token mismatch at line %d\n", linenum);
    exit(-3);
  }
}