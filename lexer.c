#include <ctype.h>
#include <lexer.h>
#include <stdio.h>
#include <string.h>

// Variavel que guarda o valor numerico do ultimo token lido
double lexval;

// Contador de linhas, util para reportar erros ou mensagens relacionadas a posicao no codigo
int linenum = 1;

// Variavel global para armazenar o lexema atual (cadeia lida pelo analisador lexico)
char lexeme[MAXIDLEN + 1];

// ASGN = [:=]
// Ex: var := 4
// Identifica o token de atribuicao ":="
int isASGN(FILE *tape) {
  lexeme[0] = getc(tape);

  // Verifica se o inicio do operador ":="
  if (lexeme[0] == ':') {
    lexeme[1] = getc(tape);

    // Confirma o operador ":="
    if (lexeme[1] == '=') {
      lexeme[2] = 0;
      return ASGN;
    }
    ungetc(lexeme[1], tape);
  }
  ungetc(lexeme[0], tape);
  lexeme[0] = 0;
  return 0;
}

// ID = [A-Za-z][A-Za-z0-9]*
// Ex: abacaXI123
// Identifica identificadores seguindo o padrao ID 
// Inclui reconhecimento especial para "quit" e "exit" (case-insensitive)
int isID(FILE *tape) {
  int i = 0;
  lexeme[i] = getc(tape);

  if (isalpha(lexeme[i])) {
    i++;
    while (isalnum(lexeme[i] = getc(tape)))
      i++;

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;

    // Converte o lexema para letras minusculas para comparacao
    char lowlex[sizeof(lexeme)];
    for (int j = 0; lexeme[j] != '\0'; j++) {
      lowlex[j] = tolower(lexeme[j]);
      lowlex[j + 1] = '\0';
    }

    if (strcmp(lowlex, "quit") == 0)
      return QUIT;
    else if (strcmp(lowlex, "exit") == 0)
      return EXIT;
    return ID;
  }

  ungetc(lexeme[i], tape);
  lexeme[i] = 0;
  return 0;
}

// OCT = 0[0-7]+
// Ex: 01, 02, 03, 04, 011
// Identifica numeros no formato OCT
int isOCT(FILE *tape){
	int i = 0;
	lexeme[i] = getc(tape);

  // Verifica se começa com '0'
	if (lexeme[i] == '0'){
		i++;
		while (isdigit(lexeme[i] = getc(tape))){
      // Verifica se corresponde a um digito OCT
			if (lexeme[i] < '0' || lexeme[i] > '7'){
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return 0;
			}
			i++;
		};

		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return OCT;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

// HEX = 0[xX][0-9a-fA-F]+
// Ex: 0x1A, 0X2F
// Identifica numeros no formato HEX
int isHEX(FILE *tape){
	int i = 0;
 	lexeme[i] = getc(tape);

  // Verifica se comeca com '0'
 	if (lexeme[i] == '0') {
		i++;
 		if (!isspace(lexeme[i] = getc(tape))){
      
      // Confirmacao de hexadecimal
 			if (lexeme[i] == 'x' || lexeme[i] == 'X'){
				i++;
 				while(isxdigit(lexeme[i] = getc(tape))) i++;

				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return HEX;
 			}
			ungetc(lexeme[i], tape);
 		}
		i--;
 	}

 	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
 	return 0;
}

// uint = [1-9][0-9]* | 0
// Enotation = [eE]['+''-']?[0-9]+
// float = (uint.[0-9]*|'.'[0-9]+)(Enotation)? | uint Enotation
// NUM = uint | float
// Identifica numeros no formato NUM
int isNUM(FILE *tape) {
  int i = 0, j = 0;
  lexeme[i] = getc(tape);

  // Adiciona '0' se começar com '.'
  if (lexeme[i] == '.') {
    lexeme[i] = '0';
    i++;
    lexeme[i] = '.';
  }

  // Verifica digito
  if (isdigit(lexeme[0])) {
    i++;

    while (isdigit(lexeme[i] = getc(tape))) {
      i++;
    }

    // Verifica Float
    if (lexeme[i] == '.') {
      i++;
      j = i;
      while (isdigit(lexeme[i] = getc(tape))) {
        i++;
      }

      // Valida digitos depois do ponto
      if (j == i) {
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        return 0;
      }
    }

    // Verifica Enotation
    if (lexeme[i] == 'e' || lexeme[i] == 'E') {
      i++;
      lexeme[i] = getc(tape);

      // Verifica sinal do expoente
      if (lexeme[i] == '+' || lexeme[i] == '-') {
        i++;
      } else {
        ungetc(lexeme[i], tape);
      }

      j = i;
      while (isdigit(lexeme[i] = getc(tape))) {
        i++;
      }

      // Faz a validacao
      if ((j == i || (!isspace(lexeme[i]) && lexeme[i] != '\n')) &&
          lexeme[i] != '+' && lexeme[i] != '-' && lexeme[i] != '*' &&
          lexeme[i] != '/' && lexeme[i] != ')') {
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        return 0;
      }
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return NUM;
  }
  ungetc(lexeme[i], tape);
  lexeme[i] = 0;
  return 0;
}

// Ignora caracteres de espaco em branco no fluxo de entrada (incluindo espacos, tabs e novas linhas)
void skipspaces(FILE *tape) {
  int head, prev_head = EOF;

  while (isspace(head = getc(tape))) {
    if (head == '\n') {
      linenum++;
      if (tape == stdin)
        break;
    }
    if (head == prev_head) {
      break;
    }
    prev_head = head;
  }

  if (head != EOF) {
    ungetc(head, tape);
  }
}

// Identifica e retorna token do fluxo de entrada
int gettoken(FILE *source) {
  int token;

  skipspaces(source);

  if ((token = isASGN(source)))
    return token;
  if ((token = isID(source)))
    return token;
  if ((token = isOCT(source)))
    return 0;
  if ((token = isHEX(source)))
    return 0;
  if ((token = isNUM(source)))
    return token;

  token = getc(source);

  return token;
}