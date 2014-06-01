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
#include "ext/standard/php_string.h"
#include "ext/pcre/php_pcre.h"
#include "aop.h"
#include "aop_joinpoint.h"


PHP_METHOD(AopJoinpoint, getPropertyName){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY)) {
        zend_error(E_ERROR, "getPropertyName is only available when the JoinPoint is a property operation (read or write)"); 
    }
    if (obj->member != NULL) {
        RETURN_ZVAL(obj->member, 1, 0);
        return; 
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getPropertyValue){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval **to_return;
#if ZEND_MODULE_API_NO >= 20100525
    zend_literal *key = NULL;
#endif

    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY)) {
        zend_error(E_ERROR, "getPropertyValue is only available when the JoinPoint is a property operation (read or write)"); 
    }
    if (obj->object!=NULL && obj->member!=NULL) {
       to_return = zend_std_get_property_ptr_ptr_overload(obj->object, obj->member AOP_KEY_C TSRMLS_CC);
    }
    RETURN_ZVAL (*to_return, 1, 0);
}

PHP_METHOD(AopJoinpoint, getArguments){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getArguments is only available when the JoinPoint is a function or method call"); 
    }
    if (obj->args == NULL) {
        obj->args = get_current_args(obj->ex TSRMLS_CC);
    }
    if (obj->args != NULL) {
        RETURN_ZVAL(obj->args, 1, 0);
    }   
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, setArguments){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "setArguments is only available when the JoinPoint is a function or ia method call"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
        zend_error(E_ERROR, "setArguments expects an array as its first argument");
        return;
    }
    if (obj->args != NULL) {
        zval_ptr_dtor(&obj->args);
    }
    obj->args = params;
    obj->args_overloaded = 1;
    Z_ADDREF_P(params);
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getKindOfAdvice){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_LONG(obj->kind_of_advice);
}

PHP_METHOD(AopJoinpoint, getPointcut){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_STRING(obj->current_pointcut->selector, 1);

}

PHP_METHOD(AopJoinpoint, getReturnedValue){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getReturnedValue is not available when the JoinPoint is a property operation (read or write)"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_BEFORE) {
        zend_error(E_ERROR, "getReturnedValue is not available when the advice was added with aop_add_before");
    }
    if ((*obj->to_return_ptr_ptr) != NULL) {
        zval_ptr_dtor (return_value_ptr);
        (*return_value_ptr) = (*obj->to_return_ptr_ptr);
        Z_ADDREF_P(*obj->to_return_ptr_ptr);
    }
    //    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getAssignedValue){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->kind_of_advice & AOP_KIND_WRITE)) {
        zend_error(E_ERROR, "getAssignedValue is only available when the JoinPoint is a property write operation"); 
    }
    if (obj->value != NULL) {
        zval_ptr_dtor (return_value_ptr);
        *return_value_ptr = obj->value;
        Z_ADDREF_P(obj->value);
    } else {
        RETURN_NULL();
    } 
}

PHP_METHOD(AopJoinpoint, setAssignedValue){
    zval *ret;
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->kind_of_advice & AOP_KIND_READ) {
        zend_error(E_ERROR, "setAssignedValue is not available when the JoinPoint is a property read operation"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
        zend_error(E_ERROR, "Error");
        return;
    } 
    if (obj->value!=NULL) {
        zval_ptr_dtor(&obj->value);
    }
    obj->value = ret;
    Z_ADDREF_P(ret);
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, setReturnedValue){
    zval *ret;
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->kind_of_advice & AOP_KIND_WRITE) {
        zend_error(E_ERROR, "setReturnedValue is not available when the JoinPoint is a property write operation"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
        zend_error(E_ERROR, "Error");
        return;
    }
    if (!(obj->kind_of_advice & AOP_KIND_PROPERTY) && *obj->to_return_ptr_ptr!=NULL) {
        zval_ptr_dtor(obj->to_return_ptr_ptr);
    }
    
    obj->value = ret;
    //If returning getObject need 2 ref
    Z_ADDREF_P(ret);
    Z_ADDREF_P(ret);
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getObject) {
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->object != NULL) {
        RETURN_ZVAL(obj->object, 1, 0);
    }
    RETURN_NULL();

}

PHP_METHOD(AopJoinpoint, getClassName){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        if (obj->object != NULL) {
            zend_class_entry *ce = Z_OBJCE_P(obj->object);
            RETURN_STRING(ce->name, 1);
        }
    } else {
        zend_class_entry *ce;
        zend_execute_data *data = obj->ex;
        zend_function *curr_func;
        if (data == NULL) {
            RETURN_NULL();
        }
        curr_func = data->function_state.function;
        ce = curr_func->common.scope;
        if (ce != NULL) {
            RETURN_STRING(ce->name, 1);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getFunctionName){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zend_execute_data *data = obj->ex;
    zend_function *curr_func;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY || obj->current_pointcut->kind_of_advice & AOP_KIND_METHOD) {
        zend_error(E_ERROR, "getMethodName is only available when the JoinPoint is a function call"); 
    }
    if (data == NULL) {
        RETURN_NULL();
    }
    curr_func = data->function_state.function;
    RETURN_STRING(curr_func->common.function_name, 1);
}

PHP_METHOD(AopJoinpoint, getException){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_CATCH)){
        zend_error(E_ERROR, "getException is only available when the advice was added with aop_add_after or aop_add_after_throwing"); 
    }
    if (obj->exception != NULL) {
        RETURN_ZVAL(obj->exception, 1, 0);
    }
    RETURN_NULL();
}



PHP_METHOD(AopJoinpoint, getMethodName){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zend_execute_data *data = obj->ex;
    zend_function *curr_func;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY || obj->current_pointcut->kind_of_advice & AOP_KIND_FUNCTION) {
        zend_error(E_ERROR, "getMethodName is only available when the JoinPoint is a method call"); 
    }
    if (data == NULL) {
        RETURN_NULL();
    }
    curr_func = data->function_state.function;
    RETURN_STRING(curr_func->common.function_name, 1);
}

PHP_METHOD(AopJoinpoint, process){
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!obj || !obj->current_pointcut || !obj->current_pointcut->kind_of_advice) {
        zend_error(E_ERROR, "Error");
    }
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_AROUND)) {
        zend_error(E_ERROR, "process is only available when the advice was added with aop_add_around"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        if (obj->kind_of_advice & AOP_KIND_WRITE) {
#if ZEND_MODULE_API_NO >= 20100525
            //            zend_literal *key = obj->key;
            // NULL for no segfault (use by zend_get_property_info_quick)
            zend_literal *key = NULL;
#endif
            _test_write_pointcut_and_execute(obj->pos, obj->advice, obj->object, obj->member, obj->value, obj->scope AOP_KEY_C);
        } else {
#if ZEND_MODULE_API_NO >= 20100525
            //            zend_literal *key = obj->key;
            // NULL for no segfault (use by zend_get_property_info_quick)
            zend_literal *key = NULL;
#endif
            obj->value = _test_read_pointcut_and_execute(obj->pos, obj->advice, obj->object, obj->member, obj->type, obj->scope AOP_KEY_C);
        }
    } else {
        _test_func_pointcut_and_execute(obj->pos, obj->advice, obj->ex, obj->object, obj->scope, obj->called_scope, obj->args_overloaded, obj->args, obj->to_return_ptr_ptr);
        obj->value = (*obj->to_return_ptr_ptr);
        if (!EG(exception)) {
            if ((*obj->to_return_ptr_ptr) != NULL) {
                RETURN_ZVAL((*obj->to_return_ptr_ptr), 1, 0);
            } else {
                RETURN_NULL();
            }
        } else {
            RETURN_NULL();
        }
    }
}
