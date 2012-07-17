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

#define PHP_AOP_VERSION "0.1.0"
#define PHP_AOP_EXTNAME "aop"

#define AOP_KIND_AROUND 1
#define AOP_KIND_BEFORE 2
#define AOP_KIND_AFTER  4
#define AOP_KIND_READ 8
#define AOP_KIND_WRITE 16
#define AOP_KIND_PROPERTY 32
#define AOP_KIND_METHOD 64
#define AOP_KIND_FUNCTION 128

#define AOP_KIND_AROUND_READ_PROPERTY (AOP_KIND_AROUND+AOP_KIND_READ+AOP_KIND_PROPERTY)
#define AOP_KIND_AROUND_WRITE_PROPERTY (AOP_KIND_AROUND+AOP_KIND_WRITE+AOP_KIND_PROPERTY)
#define AOP_KIND_BEFORE_READ_PROPERTY (AOP_KIND_BEFORE+AOP_KIND_READ+AOP_KIND_PROPERTY)
#define AOP_KIND_BEFORE_WRITE_PROPERTY (AOP_KIND_BEFORE+AOP_KIND_WRITE+AOP_KIND_PROPERTY)
#define AOP_KIND_AFTER_READ_PROPERTY (AOP_KIND_AFTER+AOP_KIND_READ+AOP_KIND_PROPERTY)
#define AOP_KIND_AFTER_WRITE_PROPERTY (AOP_KIND_AFTER+AOP_KIND_WRITE+AOP_KIND_PROPERTY)

#define AOP_KIND_AROUND_METHOD (AOP_KIND_AROUND+AOP_KIND_METHOD)
#define AOP_KIND_AROUND_FUNCTION (AOP_KIND_AROUND+AOP_KIND_FUNCTION)
#define AOP_KIND_BEFORE_METHOD (AOP_KIND_BEFORE+AOP_KIND_METHOD)
#define AOP_KIND_BEFORE_FUNCTION (AOP_KIND_BEFORE+AOP_KIND_FUNCTION)
#define AOP_KIND_AFTER_METHOD (AOP_KIND_AFTER+AOP_KIND_METHOD)
#define AOP_KIND_AFTER_FUNCTION (AOP_KIND_AFTER+AOP_KIND_FUNCTION)


#if ZEND_MODULE_API_NO >= 20100525
#define AOP_KEY_D    , const zend_literal *key
#define AOP_KEY_C    , key
#else
#define AOP_KEY_D    
#define AOP_KEY_C    
#endif

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
    zval *property;
    zval *value;
#if ZEND_MODULE_API_NO >= 20100525
    const zend_literal *key;
#endif
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
    pcre *re_method;
    pcre *re_class;
    char *regexp_method;
    char *regexp_class;
} pointcut;


typedef struct {
    int count;
    pointcut **pointcuts_cache;
    int declare_count;
    zend_class_entry *ce;
} pointcut_cache;

typedef struct {
    pointcut *pc;
    pointcut *previous_pc;
    zval *object;
} instance_of_pointcut;

typedef struct {
    HashTable *ht;
} handled_ht;

typedef struct {
    zend_object std;
    joinpoint_context *context;
    instance_of_pointcut *pc;
    instance_of_pointcut *current_pc;
    pointcut *current_pointcut;
    int current_pointcut_index;
    zval *object;
    zval *member;
    zval *value;
    int type;
#if ZEND_MODULE_API_NO >= 20100525
    const zend_literal *key;
#endif
    zend_execute_data *ex;
    zend_class_entry *scope;
    zend_class_entry *called_scope;
    zval *args;
    int args_overloaded;
    zval **to_return_ptr_ptr;
}  AopJoinpoint_object;

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(aop)
pointcut **pcs;
int count_pcs;
int overloaded;

int count_write_property;
int lock_write_property;
pointcut **property_pointcuts_write;

int count_read_property;
int lock_read_property;
pointcut **property_pointcuts_read;

int count_aopJoinpoint_cache;
zval **aopJoinpoint_cache;


handled_ht **cache_read_properties;
int cache_read_size;

handled_ht **cache_write_properties;
int cache_write_size;

ZEND_END_MODULE_GLOBALS(aop)

#ifdef ZTS
#define aop_g(v) TSRMG(aop_globals_id, zend_aop_globals *, v)
#else
#define aop_g(v) (aop_globals.v)
#endif

PHP_MINIT_FUNCTION(aop);
PHP_RINIT_FUNCTION(aop);

PHP_FUNCTION(aop_add_around);
PHP_FUNCTION(aop_add_before);
PHP_FUNCTION(aop_add_after);

PHP_METHOD(AopJoinpoint, getArguments);
PHP_METHOD(AopJoinpoint, getPropertyName);
PHP_METHOD(AopJoinpoint, setArguments);
PHP_METHOD(AopJoinpoint, getKindOfAdvice);
PHP_METHOD(AopJoinpoint, getReturnedValue);
PHP_METHOD(AopJoinpoint, getAssignedValue);
PHP_METHOD(AopJoinpoint, setReturnedValue);
PHP_METHOD(AopJoinpoint, setAssignedValue);
PHP_METHOD(AopJoinpoint, getPointcut);
PHP_METHOD(AopJoinpoint, getObject);
PHP_METHOD(AopJoinpoint, getClassName);
PHP_METHOD(AopJoinpoint, getMethodName);
PHP_METHOD(AopJoinpoint, getFunctionName);
PHP_METHOD(AopJoinpoint, process);

extern zend_module_entry aop_module_entry;
#define phpext_aop_ptr &aop_module_entry

#endif
static void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);
static void (*_zend_execute_internal) (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static void add_pointcut (zend_fcall_info fci, zend_fcall_info_cache fcic, char *selector, int selector_len, int type TSRMLS_DC);
static void parse_pointcut (pointcut **pc);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
void joinpoint_execute (instance_of_pointcut *pc);
static zval *get_current_args (TSRMLS_DC);
void exec(AopJoinpoint_object *obj TSRMLS_DC);
static int strcmp_with_joker (char *str_with_jok, char *str);
static int strcmp_with_joker_case (char *str_with_jok, char *str, int case_sensitive);
static int is_static (char *str);
static int explode_scope_by_pipe (char *partial);
static int get_scope (char *str);
static char* get_class_part (char *str);
static char * get_method_part (char *str);
void aop_execute_global (int internal, zend_op_array *ops,zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
static int pointcut_match_zend_class_entry (pointcut *pc, zend_class_entry *ce);
static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func, zend_execute_data *data);
#if ZEND_MODULE_API_NO < 20100525
static void (*zend_std_write_property)(zval *object, zval *member, zval *value TSRMLS_DC);
#endif
static zval * (*zend_std_read_property)(zval *object, zval *member, int type AOP_KEY_D TSRMLS_DC);
static zval ** (*zend_std_get_property_ptr_ptr)(zval *object, zval *member AOP_KEY_D TSRMLS_DC);
static zval * test_read_pointcut_and_execute(int current_pointcut_index, zval *object, zval *member, int type AOP_KEY_D);
static void test_write_pointcut_and_execute(int current_pointcut_index, zval *object, zval *member, zval *value AOP_KEY_D);
static void aop_add_read (char *selector, zend_fcall_info fci, zend_fcall_info_cache fcic, int type);
static void aop_add_write (char *selector, zend_fcall_info fci, zend_fcall_info_cache fcic, int type);
static void execute_pointcut (pointcut *pointcut_to_execute, zval *arg);
static int test_property_scope (pointcut *current_pc, zend_class_entry *ce, zval *member AOP_KEY_D);
static zval *execute_context (zend_execute_data *ex, zval *object, zend_class_entry *calling_scope, zend_class_entry *called_scope, int args_overloaded, zval *args);
