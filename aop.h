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


#define PHP_AOP_VERSION "0.3.0"
#define PHP_AOP_EXTNAME "aop"

#define AOP_KIND_AROUND 1
#define AOP_KIND_BEFORE 2
#define AOP_KIND_AFTER  4
#define AOP_KIND_METHOD 8 
#define AOP_KIND_FUNCTION 16
#define AOP_KIND_CATCH 32
#define AOP_KIND_RETURN 64

#define AOP_KIND_AROUND_METHOD (AOP_KIND_AROUND+AOP_KIND_METHOD)
#define AOP_KIND_AROUND_FUNCTION (AOP_KIND_AROUND+AOP_KIND_FUNCTION)
#define AOP_KIND_BEFORE_METHOD (AOP_KIND_BEFORE+AOP_KIND_METHOD)
#define AOP_KIND_BEFORE_FUNCTION (AOP_KIND_BEFORE+AOP_KIND_FUNCTION)
#define AOP_KIND_AFTER_METHOD (AOP_KIND_AFTER+AOP_KIND_METHOD)
#define AOP_KIND_AFTER_FUNCTION (AOP_KIND_AFTER+AOP_KIND_FUNCTION)


typedef struct  {
   zend_string *funcname;
   zend_object std;
} aop_joinpoint_object;

typedef struct {
	zend_function *func_ptr;
	zval obj;
	zval closure;
	zend_class_entry *ce;
    zend_string *funcname;
} aop_func_info;

typedef struct {
    int scope;
    int static_state;
    zend_string *class_name;
    int class_jok;
    zend_string *method;
    int method_jok;
    zend_string *selector;
    int kind_of_advice;
    pcre *re_method;
    pcre *re_class;
    aop_func_info *function_info;
} pointcut;



ZEND_BEGIN_MODULE_GLOBALS(aop)
    zend_bool aop_enable;
    aop_func_info alfi;
    zend_string *funcname;
    pointcut *curr;
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

aop_joinpoint_object * php_aop_joinpoint_object_fetch_object(zend_object *obj); 

void (*aop_old_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
void aop_execute_ex(zend_execute_data *execute_data TSRMLS_DC);

void (*aop_old_execute_internal)(zend_execute_data *current_execute_data, zval *return_value);
void aop_execute_internal(zend_execute_data *current_execute_data, zval *return_value);

extern zend_module_entry aop_module_entry;
#define phpext_aop_ptr &aop_module_entry


#endif
