//--------------------------------------------------------- ./scanner.h

#ifndef _LEXER_H
# define _LEXER_H
#define SCANNER_RETCODE_EOF -1
#define SCANNER_RETCODE_ERR -2
#define SCANNER_RETCODE_IMPOSSIBLE -3

#define TOKEN_SPACE 0
#define TOKEN_FUNCTION 1
#define TOKEN_CLASS 2
#define TOKEN_JOKER 3
#define TOKEN_SUPER_JOKER 4
#define TOKEN_PROPERTY 5
#define TOKEN_SCOPE 6
#define TOKEN_STATIC 7
#define TOKEN_OR 8
#define TOKEN_TEXT 9


typedef struct _scanner_state {
	char* start;
	char* end;
	char* marker;
} scanner_state;

typedef struct _scanner_token {
    int TOKEN;
    char *str_val;
    int int_val;

} scanner_token;

int scan(scanner_state *state, scanner_token *token);
//TODO: some functions to manipulate scanner states

#endif
