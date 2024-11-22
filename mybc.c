#include <lexer.h>
#include <parser.h>
#include <stdio.h>

FILE *source;

int main() {
  source = stdin;
  lookahead = gettoken(source);
  mybc();
  printf("\n");
  return 0;
}
