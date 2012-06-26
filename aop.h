/*
/+----------------------------------------------------------------------+
 | AOP                                                                  |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012 Julien Salleyron, Gérald Croës                    |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt.                                 |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Julien Salleyron <julien.salleyron@gmail.com>                |
 +----------------------------------------------------------------------+
*/
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

#define AOP_KIND_AROUND 1
#define AOP_KIND_BEFORE 2
#define AOP_KIND_AFTER  3

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
    zend_fcall_info fci;
    zend_fcall_info_cache fcic;
} pointcut;

typedef struct {
    pointcut *pc;
    pointcut *previous_pc;
    zval *object;
} instance_of_pointcut;

typedef struct {
    zend_object std;
    joinpoint_context *context;
    instance_of_pointcut *pc;
    instance_of_pointcut *current_pc;
}  aopTriggeredJoinpoint_object;

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

PHP_FUNCTION(aop_add_around);
PHP_FUNCTION(aop_add_before);
PHP_FUNCTION(aop_add_after);
PHP_FUNCTION(aop_add_final);
PHP_FUNCTION(aop_add_exception);

PHP_METHOD(aopTriggeredJoinpoint, getArguments);
PHP_METHOD(aopTriggeredJoinpoint, setArguments);
PHP_METHOD(aopTriggeredJoinpoint, getKindOfAdvice);
PHP_METHOD(aopTriggeredJoinpoint, getReturnedValue);
PHP_METHOD(aopTriggeredJoinpoint, setReturnedValue);
PHP_METHOD(aopTriggeredJoinpoint, getPointcut);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringObject);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringClassName);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringMethodName);
PHP_METHOD(aopTriggeredJoinpoint, process);

extern zend_module_entry aop_module_entry;
#define phpext_aop_ptr &aop_module_entry

#endif
static void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);
static void (*_zend_execute_internal) (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static void add_pointcut (zval *callback, char *selector, int selector_len, int type TSRMLS_DC);
static void parse_pointcut (pointcut **pc);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
void joinpoint_execute (instance_of_pointcut *pc);
static zval *get_current_args (zend_op_array *ops TSRMLS_DC);
void exec(aopTriggeredJoinpoint_object *obj TSRMLS_DC);
static int strcmp_with_joker (char *str_with_jok, char *str);
static int is_static (char *str);
static int explode_scope_by_pipe (char *partial);
static int get_scope (char *str);
static char* get_class_part (char *str);
static char * get_method_part (char *str);
void aop_execute_global (int internal, zend_op_array *ops,zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static int pointcut_match_zend_class_entry (pointcut *pc, zend_class_entry *ce);
static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func);