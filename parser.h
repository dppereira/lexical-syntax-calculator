#include <stdio.h>
#include <stdlib.h>

enum {
  SYMTABSIZE = 0x100,
  STACKSIZE = 0x400,
};

extern FILE *source;

extern void match(int expected);

extern double acc;
extern int lookahead;

extern void cmd(void);
extern void mybc(void);

extern void E(void);
extern void T(void);
extern void F(void);
extern void R(void);
extern void Q(void);