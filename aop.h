#ifndef PHP_AOP_H
#define PHP_AOP_H 1

#define PHP_AOP_VERSION "0.2b"
#define PHP_AOP_EXTNAME "aop"



typedef struct {
    zend_op_array *op;
    zend_execute_data *ex;
    zend_class_entry *scope;
    zval *currentThis;
    zval ***ret;
    zval *args;
} joinpoint_context;

typedef struct {
    int scope;
    int static_state;
    int num_ns;
    char **ns;
    char *class_name;
    char *method;
    char *selector;
    int kind_of_advice;
    zval *advice_callback;
} pointcut;

typedef struct {
    zend_uint scope;
    int static_state;
    int num_ns;
    char **ns;
    char *class_name;
    char *method;
    zend_class_entry *ce;
} joinpoint;

typedef struct {
    zend_object std;
    joinpoint_context *context;
    struct instance_of_pointcut *pc;
}  aopTriggeredJoinpoint_object;

typedef struct {
    pointcut *pc;
    pointcut *previous_pc;
    zval *object;
} instance_of_pointcut;

/*
static ZEND_DLEXPORT void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
zval *joinpoint_execute (joinpoint *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
zval *exec(aopTrigerredJoinpoint_object *obj, zval *args TSRMLS_DC);
int strcmp_with_joker (char *str_with_jok, char *str);
class_struct *make_class_struct(zval *value);
class_struct *get_current_class_struct();
char *get_class_part_with_ns(char *class);
int compare_namespace (int numns1, char **ns_with_jok, int numns2,  char **ns);
*/

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(aop)
pointcut *pcs;
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
void add_pointcut (zval *callback, zval *selector,int type TSRMLS_DC);
void parse_pointcut (pointcut *pc);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
zval *joinpoint_execute (instance_of_pointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
zval *exec(aopTriggeredJoinpoint_object *obj, zval *args TSRMLS_DC);
int strcmp_with_joker (char *str_with_jok, char *str);
int compare_namespace (int numns1, char **ns_with_jok, int numns2,  char **ns);
int get_ns(char *class, char ***ns);
int get_ns_without_class(char *class, char ***ns);
int is_static (char *str);
int explode_scope_by_pipe (char *partial);
int get_scope (char *str);
char* get_class_part (char *str);
char * get_method_part (char *str);
joinpoint *get_joinpoint_from_ce(zend_class_entry *ce);
joinpoint *get_current_joinpoint();
char *get_class_part_with_ns(char *class);
int pointcut_match_joinpoint (pointcut *pc, joinpoint *jp);
