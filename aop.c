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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "main/php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/pcre/php_pcre.h"
#include "ext/spl/spl_exceptions.h"
#include "aop_joinpoint.h"
#include "aop.h"
#include "Zend/zend_operators.h"

ZEND_DECLARE_MODULE_GLOBALS(aop)

#define DEBUG_OBJECT_HANDLERS 1

void (*aop_old_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
void aop_execute_ex(zend_execute_data *execute_data TSRMLS_DC);

void (*aop_old_execute_internal)(zend_execute_data *current_execute_data, zval *return_value);
void aop_execute_internal(zend_execute_data *current_execute_data, zval *return_value);

static void php_aop_init_globals(zend_aop_globals *aop_globals)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_aop_add, 0, 0, 2) 
    ZEND_ARG_INFO(0,pointcut)
    ZEND_ARG_INFO(0,advice)
ZEND_END_ARG_INFO()


static zend_function_entry aop_functions[] =
{
    PHP_FE(aop_add_around, arginfo_aop_add)
    PHP_FE(aop_add_before,  arginfo_aop_add)
    PHP_FE(aop_add_after, arginfo_aop_add)
    PHP_FE(aop_add_after_returning, arginfo_aop_add)
    PHP_FE(aop_add_after_throwing, arginfo_aop_add)
    {NULL, NULL, NULL}
};

zend_module_entry aop_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_AOP_EXTNAME,
    aop_functions,
    PHP_MINIT(aop),
    PHP_MSHUTDOWN(aop),
    PHP_RINIT(aop),
    PHP_RSHUTDOWN(aop),
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_AOP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AOP
ZEND_GET_MODULE(aop)
#endif

ZEND_BEGIN_ARG_INFO(arginfo_aop_args_setArguments, 0)
    ZEND_ARG_ARRAY_INFO(0, arguments, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_aop_args_setReturnedValue, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_aop_args_setAssignedValue, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_aop_args_returnbyref, 0, ZEND_RETURN_REFERENCE, -1)
ZEND_END_ARG_INFO()

static const zend_function_entry aop_methods[] = {
    PHP_ME(AopJoinpoint, getArguments, arginfo_aop_args_returnbyref, 0)
    PHP_ME(AopJoinpoint, setArguments, arginfo_aop_args_setArguments, 0)
    PHP_ME(AopJoinpoint, getKindOfAdvice, NULL, 0)
    PHP_ME(AopJoinpoint, getReturnedValue, arginfo_aop_args_returnbyref, 0)
    PHP_ME(AopJoinpoint, setReturnedValue, arginfo_aop_args_setReturnedValue, 0)
    PHP_ME(AopJoinpoint, getPointcut, NULL, 0)
    PHP_ME(AopJoinpoint, getObject, NULL, 0)
    PHP_ME(AopJoinpoint, getClassName, NULL, 0)
    PHP_ME(AopJoinpoint, getMethodName, NULL, 0)
    PHP_ME(AopJoinpoint, getFunctionName, NULL, 0)
    PHP_ME(AopJoinpoint, getException, NULL, 0)
    PHP_ME(AopJoinpoint, process, NULL, 0)
    {NULL, NULL, NULL}
};

PHP_RSHUTDOWN_FUNCTION(aop)
{
    if (!Z_ISUNDEF(aop_g(alfi).obj)) {
        zval_ptr_dtor(&(aop_g(alfi).obj));
    }
    if (!Z_ISUNDEF(aop_g(alfi).closure)) {
        zval_ptr_dtor(&aop_g(alfi).closure);
    }
    zend_string_release(aop_g(alfi).funcname);
    zend_string_release(aop_g(funcname));

    return SUCCESS;
}


PHP_RINIT_FUNCTION(aop)
{
    return SUCCESS;
}

PHP_INI_BEGIN()
    STD_PHP_INI_BOOLEAN("aop.enable","1",PHP_INI_ALL, OnUpdateBool, aop_enable, zend_aop_globals, aop_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(aop)
{
    zend_class_entry ce;
    ZEND_INIT_MODULE_GLOBALS(aop, php_aop_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    INIT_CLASS_ENTRY(ce, "AopJoinpoint", aop_methods);
    //aop_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    //aop_class_entry->create_object = aop_create_handler;
    //memcpy(&AopJoinpoint_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    //AopJoinpoint_object_handlers.clone_obj = NULL;

    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE", AOP_KIND_BEFORE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER", AOP_KIND_AFTER, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND", AOP_KIND_AROUND, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_FUNCTION", AOP_KIND_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_METHOD", AOP_KIND_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_METHOD", AOP_KIND_BEFORE_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_METHOD", AOP_KIND_AFTER_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_METHOD", AOP_KIND_AROUND_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_FUNCTION", AOP_KIND_BEFORE_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_FUNCTION", AOP_KIND_AFTER_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_FUNCTION", AOP_KIND_AROUND_FUNCTION, CONST_CS | CONST_PERSISTENT);

    aop_old_execute_ex = zend_execute_ex;
    zend_execute_ex = aop_execute_ex;
    aop_old_execute_internal = zend_execute_internal;
    zend_execute_internal = aop_execute_internal;

    return SUCCESS;
}

void aop_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
    if (execute_data->func && execute_data->func->common.function_name) {
        if (aop_g(funcname)) {
            zval *retval = NULL;
            if (zend_string_equals_ci(execute_data->func->common.function_name, aop_g(funcname))) {
                zend_call_method(Z_ISUNDEF(aop_g(alfi).obj)? NULL : &(aop_g(alfi).obj), aop_g(alfi).ce, &(aop_g(alfi).func_ptr), ZSTR_VAL(aop_g(alfi).funcname), ZSTR_LEN(aop_g(alfi).funcname), retval, 0, NULL, NULL);
            } else {
            aop_old_execute_ex(execute_data TSRMLS_CC);
            }
        } else {
            aop_old_execute_ex(execute_data TSRMLS_CC);
        }
    } else {
            aop_old_execute_ex(execute_data TSRMLS_CC);
    }
}

void aop_execute_internal(zend_execute_data *current_execute_data, zval *return_value)
{
    if (aop_old_execute_internal) {
        aop_old_execute_internal(current_execute_data, return_value TSRMLS_CC);
    } else {
        execute_internal(current_execute_data, return_value TSRMLS_CC);
    }
}




PHP_FUNCTION(aop_add_around)
{	
    zend_string *func_name;
	char *error = NULL;
	zend_string *lc_name;
	zval *zcallable = NULL;
	zend_bool do_throw = 1;
	zend_bool prepend  = 0;
	zend_function *spl_func_ptr;
	zend_object *obj_ptr;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "Sz", &aop_g(funcname), &zcallable) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_STRICT, &func_name, &fcc, &error)) {
			aop_g(alfi).ce = fcc.calling_scope;
			aop_g(alfi).func_ptr = fcc.function_handler;
			obj_ptr = fcc.object;
			if (Z_TYPE_P(zcallable) == IS_ARRAY) {
				if (!obj_ptr && aop_g(alfi).func_ptr && !(aop_g(alfi).func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
					if (do_throw) {
						zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array specifies a non static method but no object (%s)", error);
					}
					if (error) {
						efree(error);
					}
					zend_string_release(func_name);
					RETURN_FALSE;
				} else if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array does not specify %s %smethod (%s)", aop_g(alfi).func_ptr ? "a callable" : "an existing", !obj_ptr ? "static " : "", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release(func_name);
				RETURN_FALSE;
			} else if (Z_TYPE_P(zcallable) == IS_STRING) {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Function '%s' not %s (%s)", ZSTR_VAL(func_name), aop_g(alfi).func_ptr ? "callable" : "found", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release(func_name);
				RETURN_FALSE;
			} else {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Illegal value passed (%s)", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release(func_name);
				RETURN_FALSE;
			}
		} 
        aop_g(alfi).funcname = zend_string_copy(func_name);
		aop_g(alfi).ce = fcc.calling_scope;
		aop_g(alfi).func_ptr = fcc.function_handler;
		obj_ptr = fcc.object;
		if (error) {
			efree(error);
		}

		if (Z_TYPE_P(zcallable) == IS_OBJECT) {
			ZVAL_COPY(&aop_g(alfi).closure, zcallable);

		} else {
			ZVAL_UNDEF(&aop_g(alfi).closure);
		}
		zend_string_release(func_name);

		if (obj_ptr && !(aop_g(alfi).func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
			/* add object id to the hash to ensure uniqueness, for more reference look at bug #40091 */
			ZVAL_OBJ(&aop_g(alfi).obj, obj_ptr);
			Z_ADDREF(aop_g(alfi).obj);
		} else {
			ZVAL_UNDEF(&aop_g(alfi).obj);
		}
	}
}

PHP_FUNCTION(aop_add_before)
{
    /*
    zend_fcall_info fci;
    zend_fcall_info_cache fcic = { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "aop_add_before() expects a string for the pointcut as a first argument and a callback as a second argument");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }
    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_BEFORE, return_value_ptr TSRMLS_CC);
    */
}

PHP_FUNCTION(aop_add_after_throwing)
{
    /*
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "aop_add_after() expects a string for the pointcut as a first argument and a callback as a second argument");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }

    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_AFTER|AOP_KIND_CATCH, return_value_ptr TSRMLS_CC);
    */

}

PHP_FUNCTION(aop_add_after_returning)
{
    /*
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "aop_add_after() expects a string for the pointcut as a first argument and a callback as a second argument");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }

    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_AFTER|AOP_KIND_RETURN, return_value_ptr TSRMLS_CC);
    */
}

PHP_FUNCTION(aop_add_after)
{
    /*
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "aop_add_after() expects a string for the pointcut as a first argument and a callback as a second argument");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }
    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_AFTER|AOP_KIND_CATCH|AOP_KIND_RETURN, return_value_ptr TSRMLS_CC);
    */
}


    PHP_MSHUTDOWN_FUNCTION(aop)
    {
        zend_execute_ex = aop_old_execute_ex;
        zend_execute_internal = aop_old_execute_internal;

        //efree(aop_g(alfi));
        /*
#if ZEND_MODULE_API_NO >= 20121212
        zend_execute_ex  = _zend_execute;
#else
        zend_execute  = _zend_execute;
#endif
        zend_execute_internal  = _zend_execute_internal;
        
        UNREGISTER_INI_ENTRIES();
        return SUCCESS;
        */
    }


