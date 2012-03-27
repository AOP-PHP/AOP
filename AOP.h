#ifndef PHP_AOP_H
#define PHP_AOP_H 1

#define PHP_AOP_VERSION "0.1b"
#define PHP_AOP_EXTNAME "AOP"

typedef struct {
    zend_uint scope;
    int static_state;
    char *class_name;
    char **ns;
    int num_ns;
    char *method;
    zend_class_entry *ce;
} class_struct;

typedef struct {
    zval *callback;
    class_struct *selector;
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
zval *pointcut_execute (ipointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
zval *exec(AOP_object *obj, zval *args TSRMLS_DC);
char* get_class_part (char *str);
char * get_method_part (char *str);
int strcmp_with_joker (char *str_with_jok, char *str);
class_struct *make_class_struct(zval *value);
class_struct *get_current_class_struct();
char *get_class_part_with_ns(char *class);
int get_ns(char *class, char ***ns);
int compare_namespace (int numns1, char **ns_with_jok, int numns2,  char **ns);
int get_ns_without_class(char *class, char ***ns);
int get_scope (char *str);
int is_static (char *str);

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(AOP)
pointcut *pcs;
int count_pcs;
int overloaded;
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

