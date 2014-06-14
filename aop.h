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

#define PHP_AOP_VERSION "0.3.0"
#define PHP_AOP_EXTNAME "aop"

#define AOP_KIND_AROUND 1
#define AOP_KIND_BEFORE 2
#define AOP_KIND_AFTER  4
#define AOP_KIND_READ 8
#define AOP_KIND_WRITE 16
#define AOP_KIND_PROPERTY 32
#define AOP_KIND_METHOD 64
#define AOP_KIND_FUNCTION 128
#define AOP_KIND_CATCH 256
#define AOP_KIND_RETURN 512

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
#define AOP_KEY_C    , NULL
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
} pointcut;

typedef struct {
    HashTable *read;
    HashTable *write;
    HashTable *func;
} object_cache;

typedef struct {
    HashTable *ht;
    int version;
    zend_class_entry *ce;
} pointcut_cache;



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
    pointcut *current_pointcut;
    //Here because we want it different from pointcut resource
    int kind_of_advice;
    int current_pointcut_index;
    HashTable *advice;
    HashPosition pos;
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
    zval *exception;
    int args_overloaded;
    zval **to_return_ptr_ptr;
}  AopJoinpoint_object;

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(aop)
int overloaded;

int in_ex;

int lock_write_property;

int lock_read_property;

int count_aopJoinpoint_cache;
zval **aopJoinpoint_cache;

HashTable *function_cache;

zend_bool aop_enable;

HashTable * pointcuts;

object_cache **object_cache;
int object_cache_size;

int pointcut_version;


ZEND_END_MODULE_GLOBALS(aop)

#ifdef ZTS
#define aop_g(v) TSRMG(aop_globals_id, zend_aop_globals *, v)
#else
#define aop_g(v) (aop_globals.v)
#endif

PHP_MINIT_FUNCTION(aop);
PHP_RINIT_FUNCTION(aop);
PHP_RSHUTDOWN_FUNCTION(aop);
PHP_MSHUTDOWN_FUNCTION(aop);

PHP_FUNCTION(aop_add_around);
PHP_FUNCTION(aop_add_before);
PHP_FUNCTION(aop_add_after);
PHP_FUNCTION(aop_add_after_returning);
PHP_FUNCTION(aop_add_after_throwing);


extern zend_module_entry aop_module_entry;
#define phpext_aop_ptr &aop_module_entry

static void (*_zend_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
static void (*_zend_execute) (zend_op_array *ops TSRMLS_DC);

#if ZEND_MODULE_API_NO < 20121113
static void (*_zend_execute_internal) (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
#else
static void (*_zend_execute_internal) (zend_execute_data *current_execute_data, struct _zend_fcall_info *fci, int return_value_used TSRMLS_DC);
#endif
static void add_pointcut (zend_fcall_info fci, zend_fcall_info_cache fcic, char *selector, int selector_len, int type, zval **return_value_ptr TSRMLS_DC);
static void free_pointcut(void *);
static void free_pointcut_cache (void *);
ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC);

#if ZEND_MODULE_API_NO >= 20121212
ZEND_DLEXPORT void aop_execute_ex (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void _zend_execute_overload(zend_op_array *ops TSRMLS_DC);
#endif

#if ZEND_MODULE_API_NO < 20121113
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC);
#else
ZEND_DLEXPORT void aop_execute_internal (zend_execute_data *current_execute_data, struct _zend_fcall_info *fci, int return_value_used TSRMLS_DC);
#endif
zval *get_current_args (zend_execute_data *ex TSRMLS_DC);
static int strcmp_with_joker (char *str_with_jok, char *str);
static int strcmp_with_joker_case (char *str_with_jok, char *str, int case_sensitive);

static int pointcut_match_zend_class_entry (pointcut *pc, zend_class_entry *ce);
static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func, zend_execute_data *data);
#if ZEND_MODULE_API_NO < 20100525
static void (*zend_std_write_property)(zval *object, zval *member, zval *value TSRMLS_DC);
#endif
void _test_func_pointcut_and_execute(HashPosition pos, HashTable *ht, zend_execute_data *ex, zval *object, zend_class_entry *scope, zend_class_entry *called_scope, int args_overloaded, zval *args, zval **to_return_ptr_ptr);
static zval * (*zend_std_read_property)(zval *object, zval *member, int type AOP_KEY_D TSRMLS_DC);
static zval ** (*zend_std_get_property_ptr_ptr)(zval *object, zval *member AOP_KEY_D TSRMLS_DC);
void _test_write_pointcut_and_execute(HashPosition pos, HashTable *ht, zval *object, zval *member, zval *value, zend_class_entry *current_scope AOP_KEY_D);
static void execute_pointcut (pointcut *pointcut_to_execute, zval *arg);
static int test_property_scope (pointcut *current_pc, zend_class_entry *ce, zval *member AOP_KEY_D);
static void execute_context (zend_execute_data *ex, zval *object, zend_class_entry *calling_scope, zend_class_entry *called_scope, int args_overloaded, zval *args, zval ** to_return_ptr_ptr);

ZEND_DLEXPORT zval **zend_std_get_property_ptr_ptr_overload(zval *object, zval *member AOP_KEY_D TSRMLS_DC); 

HashTable *calculate_function_pointcuts (zval *object, zend_execute_data *ex);
HashTable *calculate_property_pointcuts (zval *object, zval *member, int kind AOP_KEY_D);
zval *_test_read_pointcut_and_execute(HashPosition pos, HashTable *ht, zval *object, zval *member, int type, zend_class_entry *current_scope AOP_KEY_D);
void make_regexp_on_pointcut (pointcut **pc); 
object_cache *get_object_cache (zval *object);
HashTable * get_cache_property (zval *object, zval *member, int type AOP_KEY_D);
HashTable * get_cache_func (zval *object, zend_execute_data *ex);
static void free_object_cache (void * cache);

#endif
