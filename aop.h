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
    struct joinpoint *jp;
    int internal;
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
    int nb_heritage;
    struct joinpoint **heritage;
    zend_class_entry *ce;
} joinpoint;

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

typedef struct {
    zend_function *func;
    joinpoint **jp;
} cache_joinpoint;

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(aop)
pointcut **pcs;
int count_pcs;
int overloaded;

int count_cij;
cache_joinpoint **cache_internal_joinpoint;

int count_cj;
cache_joinpoint **cache_joinpoint;

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
void add_pointcut (zval *callback, zval *selector,int type TSRMLS_DC);
void parse_pointcut (pointcut **pc);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
void joinpoint_execute (instance_of_pointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
void exec(aopTriggeredJoinpoint_object *obj);
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
joinpoint *get_current_cache_joinpoint();
joinpoint *get_current_cache_internal_joinpoint();
char *get_class_part_with_ns(char *class);
int pointcut_match_joinpoint (pointcut *pc, joinpoint *jp);
void aop_execute_global (int internal, joinpoint *jp, zend_op_array *ops,zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
