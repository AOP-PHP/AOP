#ifndef PHP_AOP_H
#define PHP_AOP_H 1

#ifndef ZVAL_COPY_VALUE
#define ZVAL_COPY_VALUE(z, v)\
    do {\
        (z)->value = (v)->value;\
        Z_TYPE_P(z) = Z_TYPE_P(v);\
    } while (0)
#endif

#define PHP_AOP_VERSION "0.2b"
#define PHP_AOP_EXTNAME "aop"



typedef struct {
    zend_op_array *op;
    zend_execute_data *ex;
    zend_class_entry *scope;
    zval *currentThis;
    zval *ret;
    zval *args;
    zend_execute_data *current_execute_data;
    int return_value_used;
    int internal;
} joinpoint_context;

typedef struct {
    int scope;
    int static_state;
    char *class_name;
    int class_jok;
    char *method;
    int method_jok;
    char *selector;
    int kind_of_advice;
    zval *advice_callback;
} pointcut;


typedef struct {
    zend_object std;
    joinpoint_context *context;
    struct instance_of_pointcut *pc;
    struct instance_of_pointcut *current_pc;
}  aopTriggeredJoinpoint_object;

typedef struct {
    pointcut *pc;
    pointcut *previous_pc;
    zval *object;
} instance_of_pointcut;

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(aop)
pointcut **pcs;
int count_pcs;
int overloaded;


ZEND_END_MODULE_GLOBALS(aop)

#ifdef ZTS
#define aop_g(v) TSRMG(aop_globals_id, zend_aop_globals *, v)
#else
#define aop_g(v) (aop_globals.v)
#endif

PHP_MINIT_FUNCTION(aop);
PHP_MSHUTDOWN_FUNCTION(aop);
PHP_RINIT_FUNCTION(aop);

ZEND_FUNCTION(aop_add_around);
ZEND_FUNCTION(aop_add_before);
ZEND_FUNCTION(aop_add_after);
ZEND_FUNCTION(aop_add_final);
ZEND_FUNCTION(aop_add_exception);


extern zend_module_entry aop_module_entry;
#define phpext_aop_ptr &aop_module_entry

#endif
static ZEND_DLEXPORT void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);
static ZEND_DLEXPORT void (*_zend_execute_internal) (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static void add_pointcut (zval *callback, zval *selector,int type TSRMLS_DC);
static void parse_pointcut (pointcut **pc);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
void joinpoint_execute (instance_of_pointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
void exec(aopTriggeredJoinpoint_object *obj);
static int strcmp_with_joker (char *str_with_jok, char *str);
static int is_static (char *str);
static int explode_scope_by_pipe (char *partial);
static int get_scope (char *str);
static char* get_class_part (char *str);
static char * get_method_part (char *str);
void aop_execute_global (int internal, zend_op_array *ops,zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static int pointcut_match_zend_class_entry (pointcut *pc, zend_class_entry *ce);
static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func);

