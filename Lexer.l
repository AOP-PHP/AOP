//--------------------------------------------------------- ./scanner.l

#include <php.h>
#include "Lexer.h"
#include "ext/pcre/php_pcre.h"
#include "aop.h"

int scan(scanner_state *s, scanner_token *t) {

	// char *cursor = s->start;
	int r=SCANNER_RETCODE_IMPOSSIBLE;
	char *q=s->start;//keep initial start

#define YYCTYPE char
#define YYCURSOR (s->start)
#define YYLIMIT (s->end)
#define YYMARKER (s->marker)

	while(SCANNER_RETCODE_IMPOSSIBLE == r) {
	
		/*!re2c
		re2c:indent:top = 2;
		re2c:yyfill:enable = 0;
                LABEL = [a-zA-Z0-9_\x7f-\xff\\*]*;
                SPACE = [ \t$]*;
		'()' {
                        t->TOKEN = TOKEN_FUNCTION;
			return 0;
		}
		'->' {
                        t->TOKEN = TOKEN_CLASS;
			return 0;
		}
		'::' {
                        t->TOKEN = TOKEN_CLASS;
			return 0;
		}
                'read' {
                        t->TOKEN = TOKEN_PROPERTY;
                        t->int_val = AOP_KIND_READ;
                        return 0;
                }
                'write' {
                        t->TOKEN = TOKEN_PROPERTY;
                        t->int_val = AOP_KIND_WRITE;
                        return 0;
                }
                'public' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val = ZEND_ACC_PUBLIC;
                        return 0;
                }
                'protected' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val = ZEND_ACC_PROTECTED;
                        return 0;
                }
                'private' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val = ZEND_ACC_PRIVATE;
                        return 0;
                }
                'static' {
                        t->TOKEN = TOKEN_STATIC;
                        t->int_val = 1;
                        return 0;
                }
                "|" {
                        t->TOKEN = TOKEN_OR;
                        return 0;
                }
                '!public' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val  = ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE;
                        return 0;
                }
                '!protected' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val  = ZEND_ACC_PUBLIC | ZEND_ACC_PRIVATE;
                        return 0;
                }
                '!private' {
                        t->TOKEN = TOKEN_SCOPE;
                        t->int_val  = ZEND_ACC_PUBLIC | ZEND_ACC_PROTECTED;
                        return 0;
                }
                '!static' {
                        t->TOKEN = TOKEN_STATIC;
                        t->int_val = 0;
                        return 0;
                }
	        LABEL {
                        t->str_val = estrndup(q,YYCURSOR - q);
                        t->TOKEN = TOKEN_TEXT;
                        return 0;
                }        
                SPACE { 
                        t->TOKEN = TOKEN_SPACE;
                        return 0; 
                }
		"\000" { r = SCANNER_RETCODE_EOF; break; }

		[^] { r = SCANNER_RETCODE_ERR; break; }
		*/
	}
	return r;
}
