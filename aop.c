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
#include "aop.h"
#include "ext/standard/php_string.h"
#include "Zend/zend_operators.h"

ZEND_DECLARE_MODULE_GLOBALS(aop)

static void php_aop_init_globals(zend_aop_globals *aop_globals)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_aop_add, 0, 0, 2) 
    ZEND_ARG_INFO(0,pointcut)
    ZEND_ARG_INFO(0,callback)
ZEND_END_ARG_INFO()

static zend_function_entry aop_functions[] =
{
    PHP_FE(aop_add_around, arginfo_aop_add)
    PHP_FE(aop_add_before,  arginfo_aop_add)
    PHP_FE(aop_add_after, arginfo_aop_add)
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
    NULL,
    PHP_RINIT(aop),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_AOP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AOP
ZEND_GET_MODULE(aop)
#endif

static zend_class_entry* aop_class_entry;
static zend_class_entry* aop_const_class_entry;

zend_object_handlers AopTriggeredJoinpoint_object_handlers;

void aop_free_storage(void *object TSRMLS_DC)
{
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)object;
//    zend_hash_destroy(obj->std.properties);
//    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value aop_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zend_object_value retval;

    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)emalloc(sizeof(AopTriggeredJoinpoint_object));
    memset(obj, 0, sizeof(AopTriggeredJoinpoint_object));
    obj->std.ce = type;

//    ALLOC_HASHTABLE(obj->std.properties);
//    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

    retval.handle = zend_objects_store_put(obj, NULL,
                                           aop_free_storage, NULL TSRMLS_CC);
    retval.handlers = &AopTriggeredJoinpoint_object_handlers;

    return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_aop_args_returnbyref, 0, ZEND_RETURN_REFERENCE, -1)
ZEND_END_ARG_INFO()

static const zend_function_entry aop_methods[] = {
    PHP_ME(AopTriggeredJoinpoint, getArguments, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getTriggeringPropertyName, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, setArguments, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getKindOfAdvice, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getReturnedValue, arginfo_aop_args_returnbyref, 0)
    PHP_ME(AopTriggeredJoinpoint, getAssignedValue, arginfo_aop_args_returnbyref, 0)
    PHP_ME(AopTriggeredJoinpoint, setReturnedValue, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, setAssignedValue, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getPointcut, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getTriggeringObject, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getTriggeringClassName, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getTriggeringMethodName, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, getTriggeringFunctionName, NULL, 0)
    PHP_ME(AopTriggeredJoinpoint, process, NULL, 0)
    {NULL, NULL, NULL}
};

PHP_RINIT_FUNCTION(aop)
{
    aop_g(count_pcs)=0;
    aop_g(overloaded)=0;

    aop_g(count_write_property)=0;
    aop_g(lock_write_property)=0;

    aop_g(count_read_property)=0;
    aop_g(lock_read_property)=0;

    aop_g(count_aopTriggeringJoinpoint_cache)=0;
    return SUCCESS;
}


static zval *get_aopTriggeringJoinpoint () {
    TSRMLS_FETCH();
    int i;
    zval *aop_object;
    for (i=0;i<aop_g(count_aopTriggeringJoinpoint_cache);i++) {
        zval *aop_object = aop_g(aopTriggeringJoinpoint_cache)[i];
        if (Z_REFCOUNT_P(aop_object)==1) {
            AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(aop_object TSRMLS_CC);
            obj->value = NULL;
            #if ZEND_MODULE_API_NO >= 20100525
            obj->key = NULL;
            #endif
            obj->member = NULL;
            obj->type = 0;
            obj->object = NULL;
            Z_ADDREF_P(aop_object);
            return aop_object;
        }
    }
    aop_g(count_aopTriggeringJoinpoint_cache)++;
    if (aop_g(count_aopTriggeringJoinpoint_cache)==1) {
        aop_g(aopTriggeringJoinpoint_cache) = emalloc(sizeof(pointcut *));
    } else {
        aop_g(aopTriggeringJoinpoint_cache) = erealloc(aop_g(aopTriggeringJoinpoint_cache),aop_g(count_aopTriggeringJoinpoint_cache)*sizeof(pointcut *));
    }
    MAKE_STD_ZVAL(aop_object);
    Z_TYPE_P(aop_object) = IS_OBJECT;
    (aop_object)->value.obj = aop_create_handler(aop_class_entry TSRMLS_CC);
    aop_g(aopTriggeringJoinpoint_cache)[aop_g(count_aopTriggeringJoinpoint_cache)-1] = aop_object;
    Z_ADDREF_P(aop_object);
    return aop_object;
}

ZEND_DLEXPORT zval **zend_std_get_property_ptr_ptr_overload(zval *object, zval *member AOP_KEY_D TSRMLS_DC) {
    zend_execute_data *ex = EG(current_execute_data);
    //Test if ++
    if (ex->opline->opcode!=ZEND_PRE_INC_OBJ &&  ex->opline->opcode!=ZEND_POST_INC_OBJ && ex->opline->opcode!=ZEND_PRE_DEC_OBJ && ex->opline->opcode!=ZEND_POST_DEC_OBJ) {
        return zend_std_get_property_ptr_ptr(object, member AOP_KEY_C TSRMLS_CC);
    } else {
        // Call original to not have a notice
        zend_std_get_property_ptr_ptr(object, member AOP_KEY_C TSRMLS_CC);
        return NULL;
    }
}

ZEND_DLEXPORT zval * zend_std_read_property_overload(zval *object, zval *member, int type AOP_KEY_D TSRMLS_DC) {
    zval *to_return;
    if (aop_g(count_read_property)>0) {
        if (aop_g(lock_read_property)>25) {
            zend_error(E_ERROR, "Nested ?");
        }
        aop_g(lock_read_property)++;
        to_return = test_read_pointcut_and_execute(0, object, member, type AOP_KEY_C);
        aop_g(lock_read_property)--;
        return to_return;
    } else {
        return zend_std_read_property(object,member, type AOP_KEY_C TSRMLS_CC);
    }
}


static zval *test_read_pointcut_and_execute(int current_pointcut_index, zval *object, zval *member, int type AOP_KEY_D) {
    zval *temp,*to_return;
    zend_class_entry *scope;
    TSRMLS_FETCH();
    if (current_pointcut_index==aop_g(count_read_property)) {
        scope = EG(scope);
        temp = EG(This);
        EG(scope) = Z_OBJCE_P(object);
        EG(This) = object;
        to_return = zend_std_read_property(object,member, type AOP_KEY_C TSRMLS_CC);
        EG(This) = temp;
        EG(scope) = scope;
        return to_return;

    } else {
            zval *to_return;
            pointcut *current_pc = aop_g(property_pointcuts_read)[current_pointcut_index];
            zend_class_entry *ce = NULL;
            char *current_class_name;
            zval *aop_object;
            zval *tmp_member;
            AopTriggeredJoinpoint_object *obj;
            if (Z_TYPE_P(member)!=IS_STRING ) {
                ALLOC_ZVAL(tmp_member);
                *tmp_member = *member;
                INIT_PZVAL(tmp_member);
                zval_copy_ctor(tmp_member);
                convert_to_string(tmp_member);
                member = tmp_member;
            #if ZEND_MODULE_API_NO >= 20100525
                key = NULL;
            #endif
            }
            if (current_pc->method[0]!='*') {
                if (!strcmp_with_joker_case(current_pc->method,Z_STRVAL_P(member), 1)) {
                    return test_read_pointcut_and_execute(current_pointcut_index+1, object, member, type AOP_KEY_C);
                }
            }
            ce = Z_OBJCE_P(object);
            
            //Scope
            if (current_pc->static_state != 2 || current_pc->scope!=0) { 
                if (!test_property_scope(current_pc, ce, member AOP_KEY_C)) {
                    return test_read_pointcut_and_execute(current_pointcut_index+1, object, member, type AOP_KEY_C);
                }
            }

            current_class_name = (char *)ce->name;
            if (!pointcut_match_zend_class_entry(current_pc->class_name, current_pc->class_jok, ce)) {
                    return test_read_pointcut_and_execute(current_pointcut_index+1, object, member, type AOP_KEY_C);
            }
            aop_object = get_aopTriggeringJoinpoint();
            obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(aop_object TSRMLS_CC);
            obj->current_pointcut = current_pc;
            obj->current_pointcut_index = current_pointcut_index; 
            obj->object = object;
            obj->member = member;
            obj->type = type;
            #if ZEND_MODULE_API_NO >= 20100525
            obj->key = key;
            #endif
            
            if (current_pc->kind_of_advice & AOP_KIND_BEFORE) {
                execute_pointcut (current_pc, aop_object);
            }
            if (current_pc->kind_of_advice & AOP_KIND_AROUND) {
                execute_pointcut (current_pc, aop_object);
                to_return = obj->value;
            } else {
                to_return = test_read_pointcut_and_execute(current_pointcut_index+1, object, member, type AOP_KEY_C);
            }
            if (current_pc->kind_of_advice & AOP_KIND_AFTER) {
                execute_pointcut (current_pc, aop_object);
                if (obj->value!=NULL) {
                    to_return = obj->value;
                }
            }
            Z_DELREF_P(aop_object);
            return to_return;
    }
}
static int test_property_scope (pointcut *current_pc, zend_class_entry *ce, zval *member AOP_KEY_D) {
    zend_property_info *property_info = NULL;
    ulong h;
#if ZEND_MODULE_API_NO < 20100525
    h = zend_get_hash_value(Z_STRVAL_P(member), Z_STRLEN_P(member) + 1);
#else
    h = key ? key->hash_value : zend_get_hash_value(Z_STRVAL_P(member), Z_STRLEN_P(member) + 1);
#endif
    if (zend_hash_quick_find(&ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, h, (void **) &property_info)==SUCCESS) {
        if (property_info) {
            if (current_pc->static_state != 2) {
                if (current_pc->static_state) {
                    if (!(property_info->flags & ZEND_ACC_STATIC)) {
                        return 0;
                    }
                } else {
                    if ((property_info->flags & ZEND_ACC_STATIC)) {
                        return 0;
                    }
                }
            }       
            if (current_pc->scope != 0 && !(current_pc->scope & (property_info->flags & ZEND_ACC_PPP_MASK))) {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        if (current_pc->scope != 0 && !(current_pc->scope & ZEND_ACC_PUBLIC)) {
            return 0;
        }
        if (current_pc->static_state == 1) {
            return 0;
        }
    }
    return 1;
}

static void test_write_pointcut_and_execute(int current_pointcut_index, zval *object, zval *member, zval *value AOP_KEY_D) {
    zval *temp;
    zend_class_entry *scope;
    TSRMLS_FETCH();
    if (current_pointcut_index==aop_g(count_write_property)) {
        scope = EG(scope);
        temp = EG(This);
        EG(scope) = Z_OBJCE_P(object);
        EG(This) = object;
        zend_std_write_property(object,member,value AOP_KEY_C TSRMLS_CC);
        EG(This) = temp;
        EG(scope) = scope;
    } else {
            pointcut *current_pc = aop_g(property_pointcuts_write)[current_pointcut_index];
            zend_class_entry *ce = NULL;
            char *current_class_name;
            AopTriggeredJoinpoint_object *obj;
            zval *aop_object;
            zval *tmp_member;
            if (Z_TYPE_P(member)!=IS_STRING ) {
                ALLOC_ZVAL(tmp_member);
                *tmp_member = *member;
                INIT_PZVAL(tmp_member);
                zval_copy_ctor(tmp_member);
                convert_to_string(tmp_member);
                member = tmp_member;
            #if ZEND_MODULE_API_NO >= 20100525
                key = NULL;
            #endif
            }

            if (current_pc->method[0]!='*') {
                if (!strcmp_with_joker_case(current_pc->method,Z_STRVAL_P(member), 1)) {
                    test_write_pointcut_and_execute(current_pointcut_index+1, object, member, value AOP_KEY_C);
                    return ;
                }
            }
            ce = Z_OBJCE_P(object);
            //Scope
            if (current_pc->static_state != 2 || current_pc->scope!=0) { 
                if (!test_property_scope(current_pc, ce, member AOP_KEY_C)) {
                    test_write_pointcut_and_execute(current_pointcut_index+1, object, member, value AOP_KEY_C);
                    return ;
                }
            }
           else {
            }


            current_class_name = (char *)ce->name;

            if (!pointcut_match_zend_class_entry(current_pc->class_name, current_pc->class_jok, ce)) {
                    test_write_pointcut_and_execute(current_pointcut_index+1, object, member, value AOP_KEY_C);
                    return ;
            }
            aop_object = get_aopTriggeringJoinpoint();
            obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(aop_object TSRMLS_CC);
            obj->current_pointcut = current_pc;
            obj->current_pointcut_index = current_pointcut_index; 
            obj->object = object;
            obj->member = member;
            obj->value = value;
            #if ZEND_MODULE_API_NO >= 20100525
            obj->key = key;
            #endif
            
            if (current_pc->kind_of_advice & AOP_KIND_BEFORE) {
                execute_pointcut (current_pc, aop_object);
            }
            if (current_pc->kind_of_advice & AOP_KIND_AROUND) {
                execute_pointcut (current_pc, aop_object);
            } else {
                value = obj->value;
                test_write_pointcut_and_execute(current_pointcut_index+1, object, member, value AOP_KEY_C);
            }
            if (current_pc->kind_of_advice & AOP_KIND_AFTER) {
                execute_pointcut (current_pc, aop_object);
            }
            Z_DELREF_P(aop_object);
    }
}

static void execute_pointcut (pointcut *pointcut_to_execute, zval *arg) {
    TSRMLS_FETCH();
    zval *args[1], *zret_ptr;
    args[0] = (zval *)&(arg);
    zret_ptr = NULL;
    pointcut_to_execute->fci.param_count = 1;
    pointcut_to_execute->fci.size = sizeof(pointcut_to_execute->fci);
    pointcut_to_execute->fci.retval_ptr_ptr = &zret_ptr;
    pointcut_to_execute->fci.params = (zval ***)args;
    if (zend_call_function(&(pointcut_to_execute->fci), &(pointcut_to_execute->fcic) TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "Problem in AOP Callback");
    }
}

ZEND_DLEXPORT void zend_std_write_property_overload(zval *object, zval *member, zval *value AOP_KEY_D TSRMLS_DC) {
    if (aop_g(count_write_property)>0) {
        if (aop_g(lock_write_property)>25) {
            zend_error(E_ERROR, "Nested ?");
        }
        aop_g(lock_write_property)++;
        test_write_pointcut_and_execute(0, object, member, value AOP_KEY_C);
        aop_g(lock_write_property)--;
    } else {
        zend_std_write_property(object,member,value AOP_KEY_C TSRMLS_CC);
    }
}


PHP_MINIT_FUNCTION(aop)
{
    zend_class_entry ce;

    ZEND_INIT_MODULE_GLOBALS(aop, php_aop_init_globals, NULL);

    INIT_CLASS_ENTRY(ce, "AopTriggeredJoinpoint", aop_methods);
    aop_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

    aop_class_entry->create_object = aop_create_handler;
    memcpy(&AopTriggeredJoinpoint_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    AopTriggeredJoinpoint_object_handlers.clone_obj = NULL;

    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE", AOP_KIND_BEFORE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER", AOP_KIND_AFTER, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND", AOP_KIND_AROUND, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_PROPERTY", AOP_KIND_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_FUNCTION", AOP_KIND_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_METHOD", AOP_KIND_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_READ", AOP_KIND_READ, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_WRITE", AOP_KIND_WRITE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_WRITE_PROPERTY", AOP_KIND_AROUND_WRITE_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_READ_PROPERTY", AOP_KIND_AROUND_READ_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_WRITE_PROPERTY", AOP_KIND_BEFORE_WRITE_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_READ_PROPERTY", AOP_KIND_BEFORE_READ_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_WRITE_PROPERTY", AOP_KIND_AFTER_WRITE_PROPERTY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_READ_PROPERTY", AOP_KIND_AFTER_READ_PROPERTY, CONST_CS | CONST_PERSISTENT);

    
    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_METHOD", AOP_KIND_BEFORE_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_METHOD", AOP_KIND_AFTER_METHOD, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_METHOD", AOP_KIND_AROUND_METHOD, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("AOP_KIND_BEFORE_FUNCTION", AOP_KIND_BEFORE_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AFTER_FUNCTION", AOP_KIND_AFTER_FUNCTION, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_KIND_AROUND_FUNCTION", AOP_KIND_AROUND_FUNCTION, CONST_CS | CONST_PERSISTENT);

#if ZEND_MODULE_API_NO < 20100525
    zend_std_write_property = std_object_handlers.write_property;
#endif
    std_object_handlers.write_property = zend_std_write_property_overload;
    zend_std_read_property = std_object_handlers.read_property;
    std_object_handlers.read_property = zend_std_read_property_overload;
    
    zend_std_get_property_ptr_ptr = std_object_handlers.get_property_ptr_ptr;
    std_object_handlers.get_property_ptr_ptr = zend_std_get_property_ptr_ptr_overload;

    _zend_execute = zend_execute;
    zend_execute  = aop_execute;
    _zend_execute_internal = zend_execute_internal;
    zend_execute_internal  = aop_execute_internal;
    return SUCCESS;
}

PHP_METHOD(AopTriggeredJoinpoint, getTriggeringPropertyName){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY)) {
        zend_error(E_ERROR, "getTriggeringPropertyName is only available while using on property"); 
    }
    if (obj->member!=NULL) {
        RETURN_ZVAL(obj->member, 1, 0);
        return; 
    }
    RETURN_NULL();
}
PHP_METHOD(AopTriggeredJoinpoint, getArguments){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getArguments is not available while using on property"); 
    }
    if (obj->context->args != NULL) {
        RETURN_ZVAL(obj->context->args, 1, 0);
    }   
    RETURN_NULL();
}

PHP_METHOD(AopTriggeredJoinpoint, setArguments){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "setArguments is not available while using on property"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
        zend_error(E_ERROR, "setArguments expects an array as its first argument");
        return;
    }
    obj->context->args = params;
    Z_ADDREF_P(params);
    RETURN_NULL();
}

PHP_METHOD(AopTriggeredJoinpoint, getKindOfAdvice){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_LONG(obj->current_pointcut->kind_of_advice);
}

PHP_METHOD(AopTriggeredJoinpoint, getPointcut){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_STRING(obj->current_pointcut->selector, 1);

}
PHP_METHOD(AopTriggeredJoinpoint, getReturnedValue){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getReturnedValue is not available while using on property"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_BEFORE) {
        zend_error(E_ERROR, "getReturnedValue is not available while using aop_add_before");
    }
    if (obj->context->ret != NULL) {
        zval_ptr_dtor (return_value_ptr);
        *return_value_ptr = obj->context->ret;
        Z_ADDREF_P(obj->context->ret);
    }
    RETURN_NULL();
}
PHP_METHOD(AopTriggeredJoinpoint, getAssignedValue){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_WRITE)) {
        zend_error(E_ERROR, "getAssignedValue is only available while using on write property"); 
    }
    if (obj->value!=NULL) {
        zval_ptr_dtor (return_value_ptr);
        *return_value_ptr = obj->value;
        Z_ADDREF_P(obj->value);
    } else {
        RETURN_NULL();
    } 
}

PHP_METHOD(AopTriggeredJoinpoint, setAssignedValue){
    zval *ret;
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_READ) {
        zend_error(E_ERROR, "setReturnedValue is not available while using on read property"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
                zend_error(E_ERROR, "Error");
        return;
    } 
    obj->value = ret;
    Z_ADDREF_P(ret);
    RETURN_NULL();
}

PHP_METHOD(AopTriggeredJoinpoint, setReturnedValue){
    zval *ret;
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_WRITE) {
        zend_error(E_ERROR, "setReturnedValue is not available while using on write property"); 
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
                zend_error(E_ERROR, "Error");
        return;
    }
    
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_READ) {
        obj->value = ret;
        Z_ADDREF_P(ret);
    } else {
        obj->context->ret = ret;
        Z_ADDREF_P(ret);
    }
    RETURN_NULL();
}

PHP_METHOD(AopTriggeredJoinpoint, getTriggeringObject) {
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        if (obj->object!=NULL) {
            RETURN_ZVAL(obj->object, 1, 0);
        }
        RETURN_NULL();
    } else {
        if (obj->context->currentThis != NULL) {
            RETURN_ZVAL(obj->context->currentThis, 1, 0);
        }
        RETURN_NULL();
    }
    
}

PHP_METHOD(AopTriggeredJoinpoint, getTriggeringClassName){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        if (obj->object!=NULL) {
            zend_class_entry *ce = Z_OBJCE_P(obj->object);
            RETURN_STRING(ce->name, 1);
        }
    } else {
        zend_class_entry *ce;
        zend_execute_data *data = obj->context->ex;
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

PHP_METHOD(AopTriggeredJoinpoint, getTriggeringFunctionName){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zend_execute_data *data = obj->context->ex;
    zend_function *curr_func;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getTriggeringMethodName is not available while using on property"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_METHOD) {
        zend_error(E_ERROR, "getTriggeringFunctionName is not available while using on class"); 
    }
    if (data == NULL) {
        RETURN_NULL();
    }
    curr_func = data->function_state.function;
    RETURN_STRING(curr_func->common.function_name, 1);
}


PHP_METHOD(AopTriggeredJoinpoint, getTriggeringMethodName){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zend_execute_data *data = obj->context->ex;
    zend_function *curr_func;
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        zend_error(E_ERROR, "getTriggeringMethodName is not available while using on property"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_FUNCTION) {
        zend_error(E_ERROR, "getTriggeringMethodName is not available while using on function"); 
    }
    if (data == NULL) {
        RETURN_NULL();
    }
    curr_func = data->function_state.function;
    RETURN_STRING(curr_func->common.function_name, 1);
}

PHP_METHOD(AopTriggeredJoinpoint, process){
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!(obj->current_pointcut->kind_of_advice & AOP_KIND_AROUND)) {
        zend_error(E_ERROR, "process is only available while using on around"); 
    }
    if (obj->current_pointcut->kind_of_advice & AOP_KIND_PROPERTY) {
        if (obj->current_pointcut->kind_of_advice & AOP_KIND_WRITE) {
            #if ZEND_MODULE_API_NO >= 20100525
//            zend_literal *key = obj->key;
// NULL for no segfault (use by zend_get_property_info_quick)
            zend_literal *key = NULL;
            #endif
            test_write_pointcut_and_execute(obj->current_pointcut_index+1, obj->object, obj->member, obj->value AOP_KEY_C);
        } else {
            #if ZEND_MODULE_API_NO >= 20100525
//            zend_literal *key = obj->key;
// NULL for no segfault (use by zend_get_property_info_quick)
            zend_literal *key = NULL;
            #endif
            obj->value = test_read_pointcut_and_execute(obj->current_pointcut_index+1, obj->object, obj->member, obj->type AOP_KEY_C);
        }
    } else {
        exec(obj TSRMLS_CC);
        if (!EG(exception)) {
            if (obj->context->ret != NULL) {
                RETURN_ZVAL(obj->context->ret, 1, 0);
            } else {
                RETURN_NULL();
            }
        } else {
            RETURN_NULL();
        }
    }
}

static void add_pointcut_property (zend_fcall_info fci, zend_fcall_info_cache fcic, char *selector, int selector_len, int type TSRMLS_DC) {
    type = type|AOP_KIND_PROPERTY;
    if (selector_len>4 && !strncmp("read",selector, 4)) {
        aop_add_read(estrndup(selector+5, selector_len-5), fci, fcic, type);
    } else if (selector_len>5 && !strncmp("write",selector, 5)) {
        aop_add_write(estrndup(selector+6,selector_len-6), fci, fcic, type);
    } else {
        aop_add_read(selector, fci, fcic, type);
        aop_add_write(selector, fci, fcic, type);
    }
}
static void add_pointcut (zend_fcall_info fci, zend_fcall_info_cache fcic, char *selector, int selector_len, int type TSRMLS_DC) {
    if (selector_len>2 && (selector[selector_len-2]!='(' || selector[selector_len-1]!=')')) {
        add_pointcut_property(fci, fcic, selector, selector_len, type TSRMLS_CC);
    } else {
        pointcut *pc;
        int count;
        aop_g(count_pcs)++;
        count=aop_g(count_pcs)-1;
        if (aop_g(count_pcs)==1) {
            aop_g(pcs) = emalloc(sizeof(pointcut *));
        } else {
            aop_g(pcs) = erealloc(aop_g(pcs),aop_g(count_pcs)*sizeof(pointcut *));
        }
        pc = emalloc(sizeof(pointcut));
        pc->selector = estrdup(selector);
        pc->fci = fci;
        pc->fcic = fcic;
        pc->kind_of_advice = type;
        parse_pointcut(&pc);
        aop_g(pcs)[count] = pc;
    }
}

static void parse_pointcut (pointcut **pc) {
    char *strval;
    char *space;
    char *temp;
    (*pc)->method = NULL;
    (*pc)->class_name = NULL;
    (*pc)->scope = 0;
    (*pc)->static_state = 2;
    (*pc)->method_jok = 0;
    (*pc)->class_jok = 0;
    strval = estrndup ((*pc)->selector, strlen((*pc)->selector)-2);
    php_strtolower(strval, strlen(strval));
    (*pc)->scope = get_scope(strval);
    (*pc)->static_state = is_static(strval);
    space = strrchr(strval,' ');
    if (space != NULL) {
        strval = space+1;
    }
    temp = strstr(strval, "::");
    if (temp == NULL) {
        temp = strstr(strval, "->");
    }
    if (temp == NULL) {
        (*pc)->method = strval;
    } else {
        (*pc)->method = estrdup(temp+2);
        temp[0] = '\0';
        (*pc)->class_name = strval;
    }
    
    (*pc)->method_jok = (strchr((*pc)->method, '*') != NULL);
    if ((*pc)->class_name != NULL) {
        (*pc)->kind_of_advice = (*pc)->kind_of_advice|AOP_KIND_METHOD;
        (*pc)->class_jok = (strchr((*pc)->class_name, '*') != NULL);
    } else {
        (*pc)->kind_of_advice = (*pc)->kind_of_advice|AOP_KIND_FUNCTION;
    }
}

static void aop_add_read (char *selector, zend_fcall_info fci, zend_fcall_info_cache fcic, int type) {
    TSRMLS_FETCH();
    int nb_char;
    int count;
    char * temp;
    char *strval;
    char *space;
    pointcut *pc;
    type = type|AOP_KIND_READ;
    aop_g(count_read_property)++;
    count=aop_g(count_read_property)-1;
    if (aop_g(count_read_property)==1) {
        aop_g(property_pointcuts_read) = emalloc(sizeof(pointcut *));
    } else {
        aop_g(property_pointcuts_read) = erealloc(aop_g(property_pointcuts_read),aop_g(count_read_property)*sizeof(pointcut *));
    }
    pc = emalloc(sizeof(pointcut));

    pc->selector = estrdup(selector);
    nb_char=3;
    space = strrchr(selector,' ');
    if (space != NULL) {
        selector = space+1;
    }
    temp = strstr(selector, "::$");
    if (temp==NULL) {
        temp = strstr(selector, "->$");
    }
    if (temp==NULL) {
        temp = strstr(selector, "::");
        nb_char = 2;
    }
    if (temp==NULL) {
        temp = strstr(selector, "->");
    }
    if (temp == NULL) {
        zend_error(E_ERROR, "You must specify a class and a property");
    }

    pc->method = estrdup(temp+nb_char);
    pc->class_name = estrndup(selector, temp-selector);
    pc->kind_of_advice = type;
    pc->class_jok =  (strchr(pc->class_name, '*')!=NULL);

    pc->fci = fci;
    pc->fcic = fcic;

    strval = estrdup(pc->selector);
    php_strtolower(strval, strlen(strval));
    pc->scope = get_scope(strval);
    pc->static_state = is_static(strval);

    aop_g(property_pointcuts_read)[count]=pc;

}

static void aop_add_write (char *selector, zend_fcall_info fci, zend_fcall_info_cache fcic, int type) {
    TSRMLS_FETCH();
    int nb_char;
    int count;
    char * temp;
    char *strval;
    char *space;
    pointcut *pc;
    type = type|AOP_KIND_WRITE;
    aop_g(count_write_property)++;
    count=aop_g(count_write_property)-1;
    if (aop_g(count_write_property)==1) {
        aop_g(property_pointcuts_write) = emalloc(sizeof(pointcut *));
    } else {
        aop_g(property_pointcuts_write) = erealloc(aop_g(property_pointcuts_write),aop_g(count_write_property)*sizeof(pointcut *));
    }
    pc = emalloc(sizeof(pointcut));
    pc->selector = estrdup (selector);
    space = strrchr(selector,' ');
    if (space != NULL) {
        selector = space+1;
    }
    nb_char=3;
    temp = strstr(selector, "::$");
    if (temp==NULL) {
        temp = strstr(selector, "->$");
    }
    if (temp==NULL) {
        temp = strstr(selector, "::");
        nb_char = 2;
    }
    if (temp==NULL) {
        temp = strstr(selector, "->");
    }
    if (temp == NULL) {
        zend_error(E_ERROR, "You must specify a class and a property");
    }

    pc->method = estrdup(temp+nb_char);
    pc->class_name = estrndup(selector, temp-selector);
    pc->kind_of_advice = type;
    pc->class_jok =  (strchr(pc->class_name, '*')!=NULL);

    pc->fci = fci;
    pc->fcic = fcic;

    strval = estrdup(pc->selector);
    php_strtolower(strval, strlen(strval));
    pc->scope = get_scope(strval);
    pc->static_state = is_static(strval);

    aop_g(property_pointcuts_write)[count]=pc;

}

PHP_FUNCTION(aop_add_around)
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }
    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_AROUND TSRMLS_CC);
}

PHP_FUNCTION(aop_add_before)
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }
    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_BEFORE TSRMLS_CC);
}

PHP_FUNCTION(aop_add_after)
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    char *selector;
    int selector_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &selector, &selector_len, &fci, &fcic) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    if (fci.function_name) {
        Z_ADDREF_P(fci.function_name);
    }
    if (fci.object_ptr) {
        Z_ADDREF_P(fci.object_ptr);
    }
    add_pointcut(fci, fcic, selector, selector_len, AOP_KIND_AFTER TSRMLS_CC);
}

ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC) {
    zend_execute_data *data;
    zend_function      *curr_func = NULL;

    data = EG(current_execute_data);

    if (data) {
        curr_func = data->function_state.function;
    }
    if (curr_func == NULL || curr_func->common.function_name == NULL || aop_g(overloaded) || EG(exception)) {
        _zend_execute(ops TSRMLS_CC);
        return;
    }
    aop_execute_global(0, ops, NULL, 0 TSRMLS_CC);
}

void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC) {
    zend_execute_data *data;
    zend_function      *curr_func = NULL;

    data = EG(current_execute_data);

    if (data) {
        curr_func = data->function_state.function;
    }

    if (curr_func == NULL || curr_func->common.function_name == NULL || aop_g(overloaded) || EG(exception)) {
        if (_zend_execute_internal) {
            _zend_execute_internal(current_execute_data, return_value_used TSRMLS_CC);
        } else {
            execute_internal(current_execute_data, return_value_used TSRMLS_CC);
        }
        return;
    }
    aop_execute_global(1, NULL, current_execute_data, return_value_used TSRMLS_CC);
}

void aop_execute_global (int internal, zend_op_array *ops, zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC) {
    int i,arounded;
    instance_of_pointcut *previous_ipc;
    joinpoint_context *context = NULL;
    zval *object;
    zend_execute_data *data;
    zend_function      *curr_func = NULL;
    zval *rtr;
    AopTriggeredJoinpoint_object *obj;

    arounded=0;
    previous_ipc = NULL;
    data = EG(current_execute_data);

    if (data) {
        curr_func = data->function_state.function;
    }

    for (i=aop_g(count_pcs)-1; i>=0; i--) {
        if (pointcut_match_zend_function(aop_g(pcs)[i], curr_func)) {
            if (context == NULL) {
                context = malloc(sizeof (joinpoint_context));
                context->property = NULL;
                context->op = ops;
                context->ex = EG(current_execute_data);
                context->currentThis = EG(This);
                context->scope = EG(scope);
                context->args = get_current_args(ops TSRMLS_CC);
                MAKE_STD_ZVAL(context->ret);
                context->current_execute_data = current_execute_data;
                context->return_value_used = return_value_used;
                context->internal = internal;
            }

            arounded = 1;
            rtr = NULL;
            MAKE_STD_ZVAL(object);
            Z_TYPE_P(object) = IS_OBJECT;
            (object)->value.obj = aop_create_handler(aop_class_entry TSRMLS_CC);
            obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(object TSRMLS_CC);
            obj->context = context;
            obj->pc = previous_ipc; 
            obj->current_pointcut = aop_g(pcs)[i];
            previous_ipc = emalloc(sizeof(instance_of_pointcut));
            previous_ipc->pc = aop_g(pcs)[i];
            previous_ipc->previous_pc = previous_ipc->pc;
            previous_ipc->object = object;
            obj->current_pc = previous_ipc;
        }
    }
    if (arounded) {
        aop_g(overloaded) = 1;
        joinpoint_execute(previous_ipc);
        aop_g(overloaded) = 0;
        if (!EG(exception)) {
            if (EG(return_value_ptr_ptr)) {
	            if (context->ret != NULL) {
	                *EG(return_value_ptr_ptr) = context->ret;
	            } else {
	                zval *temp;
	                MAKE_STD_ZVAL(temp);
	                *EG(return_value_ptr_ptr) = temp;
	            }
            }
        } else {
        }
    } else {
        if (internal) {
            if (_zend_execute_internal) {
                _zend_execute_internal(current_execute_data, return_value_used TSRMLS_CC);
            } else {
                execute_internal(current_execute_data, return_value_used TSRMLS_CC);
            }
        } else {
            _zend_execute(ops TSRMLS_CC);
        }
    }
}

void joinpoint_execute (instance_of_pointcut *pc) {
    zval *args[1], *zret_ptr;
    TSRMLS_FETCH();
    AopTriggeredJoinpoint_object *obj = (AopTriggeredJoinpoint_object *)zend_object_store_get_object(pc->object TSRMLS_CC);
    if (pc->pc->kind_of_advice & AOP_KIND_AFTER) {
        exec(obj TSRMLS_CC);
    }
    if (EG(exception)) {
        return ;
    }

    args[0] = (zval *)&(pc->object);
    zret_ptr = NULL;
    pc->pc->fci.param_count = 1;
    pc->pc->fci.size = sizeof(pc->pc->fci);
    pc->pc->fci.retval_ptr_ptr = &zret_ptr;
    pc->pc->fci.params = (zval ***)args;
    if (zend_call_function(&(pc->pc->fci), &(pc->pc->fcic) TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "Problem in AOP Callback");
    }
    if (!EG(exception) && pc->pc->kind_of_advice & AOP_KIND_BEFORE) {
        exec(obj TSRMLS_CC);
    }
    if (!EG(exception) && !(pc->pc->kind_of_advice & AOP_KIND_BEFORE) && zret_ptr != NULL) {
        //See if we can return or not
        if (Z_TYPE_P(zret_ptr)!=IS_NULL) {
            obj->context->ret = zret_ptr;
        }
    }
}

void exec(AopTriggeredJoinpoint_object *obj TSRMLS_DC) {
    if (obj->pc == NULL) {
            zend_execute_data *prev_data;
            zend_execute_data execute_data;
            zend_op **original_opline_ptr;
            zend_op_array *prev_op;
            HashTable *calling_symbol_table;
            zval *prev_this;
            zend_class_entry *current_scope;
            unsigned int arg_count, i;
            zval ***params;
            zval **return_value_ptr;

            //Save previous context
            prev_op = EG(active_op_array);
            prev_data = EG(current_execute_data);
            original_opline_ptr = EG(opline_ptr);
            calling_symbol_table = EG(active_symbol_table);
            current_scope = EG(scope);

            prev_this = EG(This);

            EG(active_op_array) = (zend_op_array *) obj->context->op;
            EG(current_execute_data) = obj->context->ex;
            if (obj->context->internal) {
                execute_data = *obj->context->current_execute_data;
            } else {
                execute_data = *EG(current_execute_data);
            }
            EG(This) = obj->context->currentThis;
            EG(scope) = obj->context->scope;
            arg_count=0;
            if (Z_TYPE_P(obj->context->args) == IS_ARRAY) {
                HashPosition pos;
                zval ** temp = NULL;
                zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(obj->context->args), &pos);
                while (zend_hash_get_current_data_ex(Z_ARRVAL_P(obj->context->args), (void **)&temp, &pos) == SUCCESS) {
                    arg_count++;
                    if (arg_count == 1) {
                        params = emalloc(sizeof(zval **));
                    } else {
                        params = erealloc(params, arg_count*sizeof(zval **));
                    }
                    params[arg_count-1] = temp;
                    zend_hash_move_forward_ex(Z_ARRVAL_P(obj->context->args), &pos);
                }
            }
            if (arg_count>0) {
                //Copy from zend_call_function
                ZEND_VM_STACK_GROW_IF_NEEDED((int) arg_count + 1);
                for (i=0; i<arg_count; i++) {
                    zval *param;
                    if (ARG_SHOULD_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
                        if (!PZVAL_IS_REF(*params[i]) && Z_REFCOUNT_PP(params[i]) > 1) {
                            zval *new_zval;
            
                            if (
                                !ARG_MAY_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
                                        if (i || UNEXPECTED(ZEND_VM_STACK_ELEMETS(EG(argument_stack)) == (EG(argument_stack)->top))) {
                                            zend_vm_stack_push_nocheck((void *) (zend_uintptr_t)i TSRMLS_CC);
                                            zend_vm_stack_clear_multiple(TSRMLS_C);
                                        }
                
                                        zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
                                        i+1,
                                        EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
                                        EX(function_state).function->common.scope ? "::" : "",
                                        EX(function_state).function->common.function_name);
                                    return;
                                }
                
                                ALLOC_ZVAL(new_zval);
                                *new_zval = **params[i];
                                zval_copy_ctor(new_zval);
                                Z_SET_REFCOUNT_P(new_zval, 1);
                                Z_DELREF_PP(params[i]);
                                *params[i] = new_zval;
                            }
                            Z_ADDREF_PP(params[i]);
                            Z_SET_ISREF_PP(params[i]);
                            param = *params[i];
                        } else if (PZVAL_IS_REF(*params[i]) &&
                                   (EX(function_state).function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) == 0 ) {
                            ALLOC_ZVAL(param);
                            *param = **(params[i]);
                            INIT_PZVAL(param);
                            zval_copy_ctor(param);
                        } else if (*params[i] != &EG(uninitialized_zval)) {
                            Z_ADDREF_PP(params[i]);
                            param = *params[i];
                        } else {
                            ALLOC_ZVAL(param);
                            *param = **(params[i]);
                            INIT_PZVAL(param);
                        }
                        zend_vm_stack_push_nocheck(param TSRMLS_CC);
                    }
                    EG(current_execute_data)->function_state.arguments = zend_vm_stack_top(TSRMLS_C);
                    zend_vm_stack_push_nocheck((void*)(zend_uintptr_t)arg_count TSRMLS_CC);
                }    

                aop_g(overloaded)=0;
                if (obj->context->internal) {
                    if (_zend_execute_internal) {
                        _zend_execute_internal(obj->context->current_execute_data, 1 TSRMLS_CC);
                    } else {
                        execute_internal(obj->context->current_execute_data, 1 TSRMLS_CC);
                    }
                    //Copy from execute_internal
                    #if ZEND_MODULE_API_NO >= 20100525
                    return_value_ptr = &(*(temp_variable *)((char *) obj->context->current_execute_data->Ts + obj->context->current_execute_data->opline->result.var)).var.ptr;
                    #else
                    return_value_ptr = &(*(temp_variable *)((char *) obj->context->current_execute_data->Ts + obj->context->current_execute_data->opline->result.u.var)).var.ptr;
                    #endif
                    if (!EG(exception)) {
                        if (return_value_ptr && *return_value_ptr) {
                            obj->context->ret = *return_value_ptr;
                        }
                    } 
                } else {
                    _zend_execute(EG(active_op_array) TSRMLS_CC);
                }
                if (arg_count) {
                    zend_vm_stack_clear_multiple(TSRMLS_C);
                }
                aop_g(overloaded)=1;
                //Take previous context
                EG(This) = prev_this;
                EG(opline_ptr) = original_opline_ptr;
                EG(active_op_array) = (zend_op_array *) prev_op;
                EG(current_execute_data) = prev_data;
                EG(active_symbol_table) = calling_symbol_table;
                EG(scope)=current_scope;
                //Only if we do not have exception
                if (!EG(exception)) {
                    if (!obj->context->internal) {
                        if (EG(return_value_ptr_ptr)) {
                            obj->context->ret = (zval *)*EG(return_value_ptr_ptr);
                        }
                    }
                }
    } else {
        instance_of_pointcut *pc = obj->pc;
        joinpoint_execute(pc);        
    }
}


static int strcmp_with_joker_case(char *str_with_jok, char *str, int case_sensitive) {
    int joker = 0;
    if (str_with_jok[0] == '*') {
        if (str_with_jok[1] == '\0') {
            return 1;
        }
    }
    if (str_with_jok[0] == '*') {
        if (strlen(str)>strlen(str_with_jok)-1) {
            if (case_sensitive) {
                return !strcmp(str_with_jok+1, str+(strlen(str)-(strlen(str_with_jok)-1)));
            } else {
                return !strcasecmp(str_with_jok+1, str+(strlen(str)-(strlen(str_with_jok)-1)));
            }
        } else {
            return 0;
        }
    }
    if (str_with_jok[strlen(str_with_jok)-1] == '*') {
        if (case_sensitive) {
            return !strncmp(str_with_jok, str, strlen(str_with_jok)-1);
        } else {
            return !strncasecmp(str_with_jok, str, strlen(str_with_jok)-1);
        }
    }
    if (case_sensitive) {
        return !strcmp(str_with_jok, str);
    } else {
        return !strcasecmp(str_with_jok, str);
    }
}

static int strcmp_with_joker(char *str_with_jok, char *str) {
    return strcmp_with_joker_case (str_with_jok, str, 0);
}

static int is_static (char *str) {
    int i = 0;
    int last = 0;
    char *space;
    char *p_space;
    char *temp;
    space = strchr(str, ' ');
    p_space = str;
    while (space != NULL) {
        if (!strncmp(p_space, "static", space-p_space)) {
           return 1;
        }
        if (!strncmp(p_space, "!static", space-p_space)) {
           return 0;
        }
        temp = space+1;
        space = strchr(p_space, ' ');
        p_space = temp;
    }
    return 2;
}

static int explode_scope_by_pipe (char *partial) {
     int i = 0;
     int last = 0;
     int toReturn = 0;
        while (i< (int) strlen(partial)) {
            if (partial[i]=='|') {
                if (!strcmp(estrndup(partial+last, i-last), "public")) {
                    toReturn = toReturn | ZEND_ACC_PUBLIC;
                }
                if (!strcmp(estrndup(partial+last, i-last), "private")) {
                    toReturn = toReturn | ZEND_ACC_PRIVATE;
                }
                if (!strcmp(estrndup(partial+last, i-last), "protected")) {
                    toReturn = toReturn | ZEND_ACC_PROTECTED;
                }
                if (!strcmp(estrndup(partial+last, i-last), "!public")) {
                    toReturn = toReturn | ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE;
                }
                if (!strcmp(estrndup(partial+last, i-last), "!private")) {
                    toReturn = toReturn | ZEND_ACC_PROTECTED | ZEND_ACC_PUBLIC;
                }
                if (!strcmp(estrndup(partial+last, i-last), "!protected")) {
                    toReturn = toReturn | ZEND_ACC_PUBLIC | ZEND_ACC_PRIVATE;
                }
               last = i+1; 
            }
            i++;
        }
        if (!strcmp(estrdup(partial+last), "public")) {
            toReturn = toReturn | ZEND_ACC_PUBLIC;
        }
        if (!strcmp(estrdup(partial+last), "private")) {
            toReturn = toReturn | ZEND_ACC_PRIVATE;
        }
        if (!strcmp(estrdup(partial+last), "protected")) {
            toReturn = toReturn | ZEND_ACC_PROTECTED;
        }
        if (!strcmp(estrdup(partial+last), "!public")) {
            toReturn = toReturn | ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE;
        }
        if (!strcmp(estrdup(partial+last), "!private")) {
            toReturn = toReturn | ZEND_ACC_PROTECTED | ZEND_ACC_PUBLIC;
        }
        if (!strcmp(estrdup(partial+last), "!protected")) {
            toReturn = toReturn | ZEND_ACC_PUBLIC | ZEND_ACC_PRIVATE;
        }
    return toReturn;
}

static int get_scope (char *str) {
    int i = 0;
    int toReturn = 0;
    char *partial = NULL;
    int last = 0;
    int temp_return;
    while (i<(int) strlen(str)) {
        if (str[i]==' ') {
            partial = estrndup(str,i);
            temp_return = explode_scope_by_pipe(partial);
            if (temp_return != 0) {
                toReturn |= explode_scope_by_pipe(partial);
            }
            last = i;
        }
        i++;
    }
    return toReturn;
}

//static int pointcut_match_class_name (pointcut *pc, char * class_name) {
static int pointcut_match_class_name (char *select_class_name, int with_jok, char * class_name) {
    if (with_jok) {
        char *temp_pc_class_name = select_class_name;
        char *temp_class_name = class_name;
        char *pos = strrchr(select_class_name, '\\');
        char *pos_class;
        int start = 1;
        char *started;
        char *started_class;
        int size;
        if (pos==NULL) {
          return strcmp_with_joker(select_class_name, class_name);
        }
        pos_class = strrchr(class_name, '\\');
        if (pos_class == NULL) {
            return 0;
        }
        if (!strcmp_with_joker(pos+1, pos_class+1)) {
            return 0;
        }
        started=temp_pc_class_name;
        started_class = temp_class_name;
        while (temp_pc_class_name<pos && temp_class_name<pos_class) {
            if (temp_pc_class_name[0]=='*' && start) {
                //Case **
                if (temp_pc_class_name[1]=='*') {
                    return 1;
                }
                //Finish By *
                /*
                if (temp_pc_class_name+1==pos && strchr(strchr(temp_class_name,'\\'), '\\')==NULL) {
                    return 1;
                }
                */
                //Case * for en entire ns
                if (temp_pc_class_name[1] == '\\') {
                    temp_pc_class_name++;
                    temp_pc_class_name++;
                    temp_class_name = strchr(temp_class_name, '\\')+1;
                    start=1;
                    continue;
                } 
                size = strlen(temp_pc_class_name+1)-strlen(strchr(temp_pc_class_name, '\\'));
                if (strncasecmp(temp_pc_class_name+1, strchr(temp_class_name, '\\')-size, size)) {
                    return 0;
                }
                temp_pc_class_name = strchr(temp_pc_class_name, '\\')+1;
                temp_class_name = strchr(temp_class_name, '\\')+1;
                start = 1;
                continue;
            }
            if (temp_pc_class_name[0] == '*' && !start) {
                if (strncasecmp(started, started_class, temp_pc_class_name-started)) {
                    return 0;
                }
                temp_pc_class_name = strchr(temp_pc_class_name, '\\')+1;
                temp_class_name = strchr(temp_class_name, '\\')+1;
                continue;
            }
            if (temp_pc_class_name[0] != temp_class_name[0]) {
                return 0;
            } 
            start = 0;
            if (temp_pc_class_name[0] == '\\') {
                started = temp_pc_class_name+1;
                started_class=temp_class_name+1;
                start = 1;
            }
            temp_pc_class_name++;
            temp_class_name++;
        }
        if (temp_pc_class_name >= pos && temp_class_name >= pos_class) {
            return 1;
        }
        return 0;
    } else {
        return !strcasecmp(select_class_name, class_name);
    }
}

static int pointcut_match_zend_class_entry (char *pc_class_name, int pc_class_jok, zend_class_entry *ce) {
    int i;
    if (pointcut_match_class_name(pc_class_name,pc_class_jok, (char*) ce->name)) {
        return 1;
    }
    for (i=0;i<(int) ce->num_interfaces;i++) {
        if (pointcut_match_class_name(pc_class_name, pc_class_jok, (char*) ce->interfaces[i]->name)) {
            return 1;
        }
    }
    #if ZEND_MODULE_API_NO >= 20100525
    for (i=0;i<(int)ce->num_traits;i++) {
        if (pointcut_match_class_name(pc_class_name, pc_class_jok, (char*) ce->traits[i]->name)) {
            return 1;
        }
    }
    #endif
    ce = ce->parent;
    while (ce!=NULL) {
        if (pointcut_match_class_name(pc_class_name, pc_class_jok, (char*) ce->name)) {
            return 1;
        }
        ce = ce->parent;
    }
    return 0;
}

static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func) {
    TSRMLS_FETCH();
    zend_class_entry *ce = NULL;
    zend_execute_data *data = EG(current_execute_data);
    if (pc->static_state != 2) {
        if (pc->static_state) {
           if (!(curr_func->common.fn_flags & ZEND_ACC_STATIC)) {
                return 0;
           }
        } else {
           if ((curr_func->common.fn_flags & ZEND_ACC_STATIC)) {
                return 0;
           }
        }
    }
    if (pc->scope != 0 && !(pc->scope & (curr_func->common.fn_flags & ZEND_ACC_PPP_MASK))) {
        return 0;
    }

    if (pc->class_name == NULL && pc->method[0] == '*') {
        return 1;
    }

    if (pc->class_name == NULL && curr_func->common.scope != NULL) {    
        return 0;
    }
    if (pc->method_jok) {
        if (!strcmp_with_joker(pc->method, (char*) curr_func->common.function_name)) {
            return 0;
        }
    } else {
        if (strcasecmp(pc->method, curr_func->common.function_name)) {
            return 0;
        }
    }
    if (data != NULL) {
        if (curr_func->common.fn_flags & ZEND_ACC_STATIC) {
            ce = curr_func->common.scope;
        } else if (data->object != NULL && Z_OBJCE(*data->object) != NULL) {
            ce = Z_OBJCE(*data->object);
        }
    }
    if (ce!=NULL) {
        return pointcut_match_zend_class_entry(pc->class_name, pc->class_jok, ce);
    }
    if (pc->class_name == NULL) {
        return 1;
    }
    return 0;
}

static zval *get_current_args (zend_op_array *ops TSRMLS_DC) {
    void **p;
    int arg_count;
    int i;
    zval *return_value;
    zend_execute_data *ex;
    MAKE_STD_ZVAL(return_value);
    array_init(return_value);
    ex = EG(current_execute_data);
    if (!ex || !ex->function_state.arguments) {
        zend_error(E_WARNING, "Problem in AOP getArgs");
        return 0;
    }

    p = ex->function_state.arguments;
    arg_count = (int)(zend_uintptr_t) *p;

    for (i=0; i<arg_count; i++) {
        zval *element;
        element = *((zval **) (p-(arg_count-i)));
        zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
    }
    return return_value;
}
