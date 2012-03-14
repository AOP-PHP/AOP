#ifndef PHP_AOP_H
#define PHP_AOP_H 1

#define PHP_AOP_VERSION "0.1b"
#define PHP_AOP_EXTNAME "AOP"

typedef struct {
    zval *callback;
    char *selector;
} pointcut;

typedef struct {
	zval *callback;
	zval *object;
} ipointcut;

typedef struct {
    zend_object std;
    zval *args;
    char *funcName;
    zend_op_array *op;
    zend_execute_data *ex;
    zend_class_entry *scope;
    zval *this;
    zval ***ret;
    ipointcut *ipointcut;
    int argsOverload;
}  AOP_object;


static ZEND_DLEXPORT void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
static char *get_function_name(zend_op_array *ops TSRMLS_DC);
int compare (char *str1, char *str2 TSRMLS_DC);
zval *pointcut_execute (ipointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
zval *exec(AOP_object *obj, zval *args TSRMLS_DC);
int instance_of (char *str1, char *str2 TSRMLS_DC);
char* get_class_part (char *str);
char * get_method_part (char *str);
int strcmp_with_joker (char *str_with_jok, char *str);



#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(AOP)
	pointcut *pcs;
	int count_pcs;
ZEND_END_MODULE_GLOBALS(AOP)

#ifdef ZTS
#define AOP_G(v) TSRMG(AOP_globals_id, zend_AOP_globals *, v)
#else
#define AOP_G(v) (AOP_globals.v)
#endif

PHP_MINIT_FUNCTION(AOP);
PHP_MSHUTDOWN_FUNCTION(AOP);
PHP_RINIT_FUNCTION(AOP);

ZEND_FUNCTION(AOP_add);


extern zend_module_entry AOP_module_entry;
#define phpext_AOP_ptr &AOP_module_entry

#endif

