enum {
  ID = 1024,
  NUM,
  DEC,
  FLT,
  OCT,
  HEX,
  ASGN, // ASGN = ":="
  EXIT,
  QUIT,
  MAXIDLEN = 32
};

#include <stdio.h>

extern int gettoken(FILE *);
extern int linenum;
extern char lexeme[];