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
#include "Zend/zend_interfaces.h"

#define GET_THIS() (aop_joinpoint_object *)((char *)Z_OBJ_P(getThis()) - XtOffsetOf(aop_joinpoint_object, std));

PHP_METHOD(AopJoinpoint, getArguments){
    struct aop_joinpoint_object* obj = Z_AOP_JOINPOINT_OBJ_P(getThis());
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->args == NULL) {
        obj->args = get_current_args(obj->ex TSRMLS_CC);
    }
    if (obj->args != NULL) {
        RETURN_ZVAL(obj->args, 1, 0);
    }   
    RETURN_NULL();
    */
}

PHP_METHOD(AopJoinpoint, setArguments){
/*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params;
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
    */
}

PHP_METHOD(AopJoinpoint, getKindOfAdvice){
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_LONG(obj->kind_of_advice);
    */
}

PHP_METHOD(AopJoinpoint, getPointcut){
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_STRING(obj->current_pointcut->selector, 1);
    */

}

PHP_METHOD(AopJoinpoint, getReturnedValue){
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_BEFORE) {
        zend_error(E_ERROR, "getReturnedValue is not available when the advice was added with aop_add_before");
    }
    if ((*obj->to_return_ptr_ptr) != NULL) {
        zval_ptr_dtor (return_value_ptr);
        (*return_value_ptr) = (*obj->to_return_ptr_ptr);
        Z_ADDREF_P(*obj->to_return_ptr_ptr);
    }
    //    RETURN_NULL();
    //    */
}


PHP_METHOD(AopJoinpoint, setReturnedValue){
    /*
    zval *ret;
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
        zend_error(E_ERROR, "Error");
        return;
    }
    
    obj->value = ret;
    //If returning getObject need 2 ref
    Z_ADDREF_P(ret);
    Z_ADDREF_P(ret);
    RETURN_NULL();
    */
}

PHP_METHOD(AopJoinpoint, getObject) {
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->object != NULL) {
        RETURN_ZVAL(obj->object, 1, 0);
    }
    RETURN_NULL();
    */

}

PHP_METHOD(AopJoinpoint, getClassName){
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
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
    RETURN_NULL();
    */
}

PHP_METHOD(AopJoinpoint, getFunctionName){
    aop_joinpoint_object* this = GET_THIS();
    if (this->funcname!=NULL) {
        RETURN_STR(this->funcname);
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getException){
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_CATCH)){
        zend_error(E_ERROR, "getException is only available when the advice was added with aop_add_after or aop_add_after_throwing"); 
    }
    if (obj->exception != NULL) {
        RETURN_ZVAL(obj->exception, 1, 0);
    }
    RETURN_NULL();
    */
}



PHP_METHOD(AopJoinpoint, getMethodName){
    struct aop_joinpoint_object* obj = Z_AOP_JOINPOINT_OBJ_P(getThis());
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zend_execute_data *data = obj->ex;
    zend_function *curr_func;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_FUNCTION) {
        zend_error(E_ERROR, "getMethodName is only available when the JoinPoint is a method call"); 
    }
    if (data == NULL) {
        RETURN_NULL();
    }
    curr_func = data->function_state.function;
    RETURN_STRING(curr_func->common.function_name, 1);
    */
}

PHP_METHOD(AopJoinpoint, process){

    aop_joinpoint_object* this = GET_THIS();
    if (!(this->current_pointcut->kind_of_advice & AOP_KIND_AROUND)) {
        zend_error(E_ERROR, "process is only available when the advice was added with aop_add_around"); 
    }
    zval arg;
    aop_g(overloaded) = 1; 
    test_pointcut_and_execute(this->current_pointcut_zval_ptr+1, this->execute_data, this->original, this->retval_ptr TSRMLS_CC);
    //zend_call_method(Z_ISUNDEF(this->original->obj)? NULL : &(this->original->obj), this->original->ce, &(this->original->func_ptr), ZSTR_VAL(this->original->funcname), ZSTR_LEN(this->original->funcname), retval, 0, &arg, NULL);
    aop_g(overloaded) = 0; 
    if (this->retval_ptr!=NULL) {
        RETURN_ZVAL(this->retval_ptr, 0, 1);
    }
    
    //aop_old_execute_ex(this->current_execute_data TSRMLS_CC);
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!obj || !obj->current_pointcut || !obj->current_pointcut->kind_of_advice) {
        zend_error(E_ERROR, "Error");
    }
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_AROUND)) {
        zend_error(E_ERROR, "process is only available when the advice was added with aop_add_around"); 
    }
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
    */
}
