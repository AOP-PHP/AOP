#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "aop.h"
#include "Zend/zend_operators.h"


ZEND_DECLARE_MODULE_GLOBALS(aop)


static void php_aop_init_globals(zend_aop_globals *aop_globals)
{
}

static zend_function_entry aop_functions[] =
{
    ZEND_FE(aop_add_around, NULL)
    ZEND_FE(aop_add_before, NULL)
    ZEND_FE(aop_add_after, NULL)
    ZEND_FE(aop_add_final, NULL)
    ZEND_FE(aop_add_exception, NULL)
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

zend_object_handlers aopTriggeredJoinpoint_object_handlers;

void aop_free_storage(void *object TSRMLS_DC)
{
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)object;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}



zend_object_value aop_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)emalloc(sizeof(aopTriggeredJoinpoint_object));
    memset(obj, 0, sizeof(aopTriggeredJoinpoint_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

    retval.handle = zend_objects_store_put(obj, NULL,
                                           aop_free_storage, NULL TSRMLS_CC);
    retval.handlers = &aopTriggeredJoinpoint_object_handlers;

    return retval;
}

PHP_METHOD(aopTriggeredJoinpoint, getArguments);
PHP_METHOD(aopTriggeredJoinpoint, setArguments);
PHP_METHOD(aopTriggeredJoinpoint, getKindOfAdvice);
PHP_METHOD(aopTriggeredJoinpoint, getReturnedValue);
PHP_METHOD(aopTriggeredJoinpoint, setReturnedValue);
PHP_METHOD(aopTriggeredJoinpoint, getException);
PHP_METHOD(aopTriggeredJoinpoint, setException);
PHP_METHOD(aopTriggeredJoinpoint, getPointcut);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringObject);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringClassName);
PHP_METHOD(aopTriggeredJoinpoint, getTriggeringMethodName);
PHP_METHOD(aopTriggeredJoinpoint, process);


ZEND_BEGIN_ARG_INFO(arginfo_aop_noargs, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry aop_methods[] = {
    PHP_ME(aopTriggeredJoinpoint, getArguments,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, setArguments,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getKindOfAdvice,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getReturnedValue,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, setReturnedValue,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getException,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, setException,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getPointcut,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getTriggeringObject,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getTriggeringClassName,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, getTriggeringMethodName,arginfo_aop_noargs, 0)
    PHP_ME(aopTriggeredJoinpoint, process,arginfo_aop_noargs, 0)
    {
        NULL, NULL, NULL
    }
};

static const zend_function_entry aop_const_methods[] = {
    {
        NULL, NULL, NULL
    }
};


PHP_MSHUTDOWN_FUNCTION(aop)
{
    return SUCCESS;
}

PHP_RINIT_FUNCTION(aop)
{
    aop_g(count_pcs)=0;
    aop_g(overloaded)=0;
    return SUCCESS;
}

PHP_MINIT_FUNCTION(aop)
{
    ZEND_INIT_MODULE_GLOBALS(aop, php_aop_init_globals,NULL);
    //Using if with array
    REGISTER_LONG_CONSTANT("AOP_PRIVATE", ZEND_ACC_PRIVATE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_PUBLIC", ZEND_ACC_PUBLIC, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_PROTECTED", ZEND_ACC_PROTECTED, CONST_CS | CONST_PERSISTENT);

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "aopTriggeredJoinpoint", aop_methods);
    aop_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

    aop_class_entry->create_object = aop_create_handler;
    memcpy(&aopTriggeredJoinpoint_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    aopTriggeredJoinpoint_object_handlers.clone_obj = NULL;



    zend_class_entry aop_ce;
    INIT_CLASS_ENTRY(aop_ce, "AOP", aop_const_methods);
    aop_const_class_entry = zend_register_internal_class(&aop_ce TSRMLS_CC);

    zend_declare_class_constant_long (aop_const_class_entry,"AROUND",sizeof("AROUND")-1,(long)1 TSRMLS_CC);
    zend_declare_class_constant_long (aop_const_class_entry,"BEFORE",sizeof("BEFORE")-1,(long)2 TSRMLS_CC);
    zend_declare_class_constant_long (aop_const_class_entry,"AFTER",sizeof("AFTER")-1,(long)3 TSRMLS_CC);




    _zend_execute = zend_execute;
    zend_execute  = aop_execute;
    _zend_execute_internal = zend_execute_internal;
    zend_execute_internal  = aop_execute_internal;
    return SUCCESS;
}

PHP_METHOD(aopTriggeredJoinpoint, getArguments){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->context->args!=NULL) {
        ZVAL_COPY_VALUE(return_value, obj->context->args);
        //Z_UNSET_ISREF_P(return_value);
        Z_ADDREF_P(return_value);
    }   
}

PHP_METHOD(aopTriggeredJoinpoint, setArguments){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params; 
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
        zend_error(E_ERROR, "setArguments param must be an array");
        return;
    }
    obj->context->args = params;
    Z_ADDREF_P(obj->context->args);
    
    
}

PHP_METHOD(aopTriggeredJoinpoint, getKindOfAdvice){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    instance_of_pointcut *ipc = obj->current_pc;
    pointcut *pc = ipc->pc;
    RETURN_LONG(pc->kind_of_advice);
}

PHP_METHOD(aopTriggeredJoinpoint, getPointcut){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    instance_of_pointcut *ipc = obj->current_pc;
    pointcut *pc = ipc->pc;
    RETURN_STRING(pc->selector, 1);

}
PHP_METHOD(aopTriggeredJoinpoint, getReturnedValue){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    instance_of_pointcut *ipc = obj->current_pc;
    pointcut *pc = ipc->pc;
    if (pc->kind_of_advice==2) {
        zend_error(E_ERROR, "getReturnedValue can be call in aop_add_before");
    }
    if (obj->context->ret != NULL) {
        RETURN_ZVAL(obj->context->ret, 1, 0);
    }
}



PHP_METHOD(aopTriggeredJoinpoint, setReturnedValue){
    zval *ret;
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ret) == FAILURE) {
                zend_error(E_ERROR, "Error");
        return;
    }
    obj->context->ret = ret;
    Z_ADDREF_P(ret);
}
PHP_METHOD(aopTriggeredJoinpoint, getException){}
PHP_METHOD(aopTriggeredJoinpoint, setException){}

PHP_METHOD(aopTriggeredJoinpoint, getTriggeringObject) {
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->context->currentThis!=NULL) {
        RETURN_ZVAL(obj->context->currentThis, 1, 0);
    }

}

PHP_METHOD(aopTriggeredJoinpoint, getTriggeringClassName){}

PHP_METHOD(aopTriggeredJoinpoint, getTriggeringMethodName){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    joinpoint *jp = obj->context->jp;
    if (jp->method != NULL) {
        RETURN_STRING(jp->method, 1);
    }
}

PHP_METHOD(aopTriggeredJoinpoint, process){
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
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



void add_pointcut (zval *callback, zval *selector,int type TSRMLS_DC) {
    aop_g(count_pcs)++;
    int count=aop_g(count_pcs)-1;
    if (aop_g(count_pcs)==1) {
        aop_g(pcs) = emalloc(sizeof(pointcut *));
    } else {
        aop_g(pcs) = erealloc(aop_g(pcs),aop_g(count_pcs)*sizeof(pointcut *));
    }
    pointcut *pc = emalloc(sizeof(pointcut));
    pc->selector = estrdup(Z_STRVAL_P(selector));
    Z_ADDREF_P(callback);
    pc->advice_callback = callback;
    pc->kind_of_advice = type;
    parse_pointcut(&pc);
    aop_g(pcs)[count] = pc;
}

void parse_pointcut (pointcut **pc) {
    (*pc)->method = NULL;
    (*pc)->class_name = NULL;
    (*pc)->scope = NULL;
    (*pc)->static_state = 2;
    char *strval;
    strval = estrdup ((*pc)->selector);
    php_strtolower(strval, strlen(strval));
    (*pc)->class_name = get_class_part(strval);
    (*pc)->scope = get_scope(strval);
    (*pc)->static_state = is_static(strval);
    if ((*pc)->class_name!=NULL) {
        (*pc)->method = get_method_part(strval);
        (*pc)->num_ns = get_ns((*pc)->class_name, &((*pc)->ns));
        if ((*pc)->num_ns>0) {
            (*pc)->class_name = get_class_part_with_ns((*pc)->class_name);
        }
    } else {
        (*pc)->method = estrdup(strval);
        (*pc)->num_ns = get_ns((*pc)->method, &((*pc)->ns));
        if ((*pc)->num_ns>0) {
            (*pc)->method = get_class_part_with_ns((*pc)->method);
        }
    }
}


ZEND_FUNCTION(aop_add_around)
{
    zval *callback;
    zval *selector;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &selector, &callback) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    add_pointcut(callback, selector,1 TSRMLS_CC);
}

ZEND_FUNCTION(aop_add_before)
{
    zval *callback;
    zval *selector;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &selector, &callback) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    add_pointcut(callback, selector,2 TSRMLS_CC);
}


ZEND_FUNCTION(aop_add_after)
{
    zval *callback;
    zval *selector;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &selector, &callback) == FAILURE) {
        zend_error(E_ERROR, "Bad params");
        return;
    }
    add_pointcut(callback, selector,3 TSRMLS_CC);
}

ZEND_FUNCTION(aop_add_final) {}
ZEND_FUNCTION(aop_add_exception) {}

ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC) {
    joinpoint *jp = get_current_joinpoint ();
    if (jp==NULL || jp->method==NULL || aop_g(overloaded) || EG(exception)) {
        _zend_execute(ops TSRMLS_CC);
        return;
    }
    aop_execute_global(0, jp, ops, NULL, NULL TSRMLS_CC);
}

void aop_execute_internal (zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC) {
    joinpoint *jp = get_current_joinpoint ();
    if (jp==NULL || jp->method==NULL || aop_g(overloaded) || EG(exception)) {
        if (_zend_execute_internal) {
            _zend_execute_internal(current_execute_data,return_value_used TSRMLS_CC);
        } else {
            execute_internal(current_execute_data,return_value_used TSRMLS_CC);
        }
        return;
    }
    aop_execute_global(1, jp, NULL, current_execute_data,return_value_used TSRMLS_CC);

}

void aop_execute_global (int internal, joinpoint *jp, zend_op_array *ops, zend_execute_data *current_execute_data, int return_value_used TSRMLS_DC) {
    int i,arounded;
    arounded=0;
    instance_of_pointcut *previous_ipc;
    joinpoint_context *context = NULL;
    previous_ipc = NULL;
    zval *object;
    for (i=0;i<aop_g(count_pcs);i++) {
        if (pointcut_match_joinpoint(aop_g(pcs)[i],jp)) {
            if (context==NULL) {
                context = malloc(sizeof (joinpoint_context));
                context->op = ops;
                context->ex = EG(current_execute_data);
                context->currentThis = EG(This);
                context->scope = EG(scope);
                context->args = get_current_args(ops TSRMLS_CC);
                context->jp = jp;
                context->ret = NULL;
                context->current_execute_data = current_execute_data;
                context->return_value_used = return_value_used;
                context->internal = internal;
            }

            arounded=1;
            zval *rtr=NULL;
            MAKE_STD_ZVAL(object);
            Z_TYPE_P(object) = IS_OBJECT;
            (object)->value.obj = aop_create_handler(aop_class_entry TSRMLS_CC);
            aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(object TSRMLS_CC);
            obj->context = context;
            obj->pc = previous_ipc; 
            previous_ipc = emalloc(sizeof(instance_of_pointcut));
            previous_ipc->pc = aop_g(pcs)[i];
            previous_ipc->previous_pc = previous_ipc->pc;
            previous_ipc->object = object;
            obj->current_pc = previous_ipc;
        }
    }
    if (arounded) {
        aop_g(overloaded)=1;
        joinpoint_execute(previous_ipc);
        aop_g(overloaded)=0;
        if (!EG(exception)) {
            if (EG(return_value_ptr_ptr)) {
	            if (context->ret!=NULL) {
	                *EG(return_value_ptr_ptr)=context->ret;
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
                _zend_execute_internal(current_execute_data,return_value_used TSRMLS_CC);
            } else {
                execute_internal(current_execute_data,return_value_used TSRMLS_CC);
            }
        } else {
            _zend_execute(ops TSRMLS_CC);
        }
    }
}

void joinpoint_execute (instance_of_pointcut *pc) {
    aopTriggeredJoinpoint_object *obj = (aopTriggeredJoinpoint_object *)zend_object_store_get_object(pc->object TSRMLS_CC);
    if (pc->pc->kind_of_advice==3) {
        exec(obj TSRMLS_CC);
    }
    if (EG(exception)) {
        return ;
    }
    zval *args[1];
    args[0] = (zval *)&(pc->object);
    zval *zret_ptr=NULL;
    zend_fcall_info fci;
    zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
    TSRMLS_FETCH();
    fci.param_count= 1;
    fci.size= sizeof(fci);
    fci.function_table= EG(function_table);
    fci.function_name= pc->pc->advice_callback;
    fci.symbol_table= NULL;
    fci.retval_ptr_ptr= &zret_ptr;
    fci.params = (zval ***)args;
    fci.object_ptr= NULL;
    fci.no_separation= 0;
    if (zend_call_function(&fci, &fcic TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "Problem in AOP Callback");
    }
    if (!EG(exception) && pc->pc->kind_of_advice==2) {
        exec(obj TSRMLS_CC);
    }
    if (!EG(exception) && pc->pc->kind_of_advice!=2 && zret_ptr!=NULL) {
        if (Z_TYPE_P(zret_ptr)!=IS_NULL) {
            obj->context->ret = zret_ptr;
        }
    }
}


void exec(aopTriggeredJoinpoint_object *obj TSRMLS_DC) {
        
    if (obj->pc==NULL) {
        zend_execute_data *prev_data;
        zend_execute_data execute_data;
        zend_op **original_opline_ptr;
        zend_op_array *prev_op;
        HashTable *calling_symbol_table;
        zval *prev_this;
        zend_class_entry *current_scope;


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
        int arg_count;
        arg_count=0;
        zval ***params;
        if (Z_TYPE_P(obj->context->args)==IS_ARRAY) {
            HashPosition pos;
            zval ** temp=NULL;
            zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(obj->context->args), &pos);
            while (zend_hash_get_current_data_ex(Z_ARRVAL_P(obj->context->args), (void **)&temp, &pos)==SUCCESS) {
                arg_count++;
                if (arg_count==1) {
                    params = emalloc(sizeof(zval **));
                } else {
                    params = erealloc(params, arg_count*sizeof(zval **));
                }
                params[arg_count-1] = temp;
                zend_hash_move_forward_ex(Z_ARRVAL_P(obj->context->args), &pos);
            }
        }
if (arg_count>0) {
	ZEND_VM_STACK_GROW_IF_NEEDED(arg_count + 1);
    int i;
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
					return FAILURE;
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
            zval **return_value_ptr = &(*(temp_variable *)((char *) obj->context->current_execute_data->Ts + obj->context->current_execute_data->opline->result.var)).var.ptr;
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
                } else {
//            obj->context->ret = NULL;
//        php_printf("val %s\n",Z_STRVAL_P(obj->context->ret));
//        Z_ADDREF_P(obj->context->ret);
            //return NULL;
                }
            } else {

            }
        }

    } else {
        instance_of_pointcut *pc = obj->pc;
        joinpoint_execute(pc);        
    }

}




int strcmp_with_joker (char *str_with_jok, char *str) {
    php_strtolower(str, strlen(str));
    if (!strcmp(str_with_jok,"*")) {
        return 1;
    }

    int i;
    int joker=0;
    if (str_with_jok[strlen(str_with_jok)-1]=='*') {
        return !strcmp(estrndup(str_with_jok,strlen(str_with_jok)-2),estrndup(str,strlen(str_with_jok)-2));
    }
    if (str_with_jok[0]=='*') {
        if (strlen(str)>strlen(str_with_jok)-1) {
            return !strcmp(estrdup(str_with_jok+1),estrdup(str+(strlen(str)-(strlen(str_with_jok)-1))));
        } else {
            return 0;
        }
    }
    return !strcmp(str_with_jok,str);
}

int compare_namespace (int numns1, char **ns_with_jok, int numns2,  char **ns) {
    if (numns1==0 && numns2>0) {
        return 0;
    }
    if (numns1==0 && numns2==0) {
        return 1;
    }
    if (numns1>numns2) {
        return 0;
    }
    if (numns2>numns1 && strcmp(ns_with_jok[numns1-1],"**")) {
        return 0;
    }
    int i;
    for (i=0;i<numns1;i++) {
        if (!strcmp_with_joker(ns_with_jok[i],ns[i])) {
            return 0;
        }
    }
    return 1;
}
int get_ns(char *class, char ***ns) {
    int nb;
    nb = get_ns_without_class(class,ns);
    if (nb==1) {
        (*ns)=NULL;
    }
    
    return --nb;
}

int get_ns_without_class(char *class, char ***ns) {
    int i;
    int nb = 0;
    int last = 0;
    for (i=0;i<strlen(class);i++) {
        if (class[i]=='\\') {
            nb++;
            if (nb==1) {
                (*ns) = emalloc(sizeof(char *));
            } else {
                (*ns) = erealloc((*ns),nb*sizeof(char *));
            }
            (*ns)[nb-1] = estrndup(class+last,i-last);
            php_strtolower((*ns)[nb-1], strlen((*ns)[nb-1]));
            last = i+1;
        }
    }
    nb++;
    if (nb==1) {
        (*ns) = emalloc(sizeof(char *));
    } else {
        (*ns) = erealloc((*ns),nb*sizeof(char *));
    }
    (*ns)[nb-1] = estrndup(class+last,i-last);
    return nb;

}



int is_static (char *str) {
    int i=0;
    char *partial = NULL;
    int last = 0;
    while (i<strlen(str)) {
        if (str[i]==' ') {
            partial = estrndup(str+last,i-last);
            if (!strcmp(partial, "static")) {
                return 1;
            }
            if (!strcmp(partial, "!static")) {
                return 0;
            }
            last = i+1;
        }
        i++;
    }
    return 2;

}

int explode_scope_by_pipe (char *partial) {
     int i = 0;
     int last = 0;
     int toReturn = NULL;
        while (i<strlen(partial)) {
            if (partial[i]=='|') {
                if (!strcmp(estrndup(partial+last,i-last), "public")) {
                    toReturn = toReturn | ZEND_ACC_PUBLIC;
                }
                if (!strcmp(estrndup(partial+last,i-last), "private")) {
                    toReturn = toReturn | ZEND_ACC_PRIVATE;
                }
                if (!strcmp(estrndup(partial+last,i-last), "protected")) {
                    toReturn = toReturn | ZEND_ACC_PROTECTED;
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
    return toReturn;
}

int get_scope (char *str) {
    int i=0;
    int toReturn = NULL;
    char *partial = NULL;
    int last = 0;
    while (i<strlen(str)) {
        if (str[i]==' ') {
            partial = estrndup(str,i);
            int temp_return = explode_scope_by_pipe(partial);
            if (temp_return!=NULL) {
                toReturn |= explode_scope_by_pipe(partial);
            }
            last = i;
        }
        i++;
    }
    return toReturn;
}

char* get_class_part (char *str) {
    int i=0;
    int last_space = 0;
    while (i<strlen(str)) {
        if (str[i]==' ') {
            last_space = i+1;
        }
        i++;
    }
    str = str+last_space;
    char *endp;
    char *class_end;
    endp=str+strlen(str);
    class_end = php_memnstr(str, "::", 2, endp);
    if (class_end!=NULL) {
        return estrndup(str, strlen(str)-strlen(class_end));
    }
    
    return NULL;

}

char * get_method_part (char *str) {

    char *endp;
    endp=str+strlen(str);
    return (php_memnstr(str, "::", 2, endp)+2);
}


joinpoint *get_joinpoint_from_ce (zend_class_entry *ce) {
    joinpoint *cs = emalloc (sizeof(joinpoint));
    cs->static_state = NULL;
    cs->method=NULL;
    cs->class_name = NULL;
    cs->ce = NULL;
    cs->ce = ce;
    cs->num_ns = get_ns(ce->name, &(cs->ns));
    if (cs->num_ns>0) {
        cs->class_name = get_class_part_with_ns(cs->class_name);
    } else {
        cs->class_name = estrdup(ce->name);
    }
    php_strtolower(cs->class_name, strlen(cs->class_name));
    return cs;
}



joinpoint *get_current_joinpoint() {
    joinpoint *cs = emalloc (sizeof(joinpoint));
    cs->static_state = NULL;
    cs->method=NULL;
    cs->class_name = NULL;
    cs->ce = NULL;

    zend_execute_data *data;
    zend_function      *curr_func;

    data = EG(current_execute_data);

    if (data) {
        curr_func = data->function_state.function;
        if (curr_func->common.function_name==NULL) {
            return NULL;
        }
        cs->method = estrdup (curr_func->common.function_name);
        php_strtolower(cs->method, strlen(cs->method));
        cs->scope = curr_func->common.fn_flags;
        cs->static_state = (curr_func->common.fn_flags & ZEND_ACC_STATIC);
        if (cs->method) {
            if (cs->static_state) {
                cs->ce = curr_func->common.scope;
                cs->class_name = cs->ce->name;
            } else if (data->object) {
                cs->ce = Z_OBJCE(*data->object);
                cs->class_name = estrdup (Z_OBJCE(*data->object)->name);
            }
            if (cs->class_name!=NULL) {
                php_strtolower(cs->class_name, strlen(cs->class_name));
            }
            if (cs->ce!=NULL) {
                cs->num_ns = get_ns(cs->ce->name, &(cs->ns));
                if (cs->num_ns>0) {
                    cs->class_name = get_class_part_with_ns(cs->class_name);
                }
                zend_class_entry *ce;
                ce = cs->ce->parent;
                cs->nb_heritage = 0;
                while (ce!=NULL) {
                    cs->nb_heritage++;
                    if (cs->nb_heritage==1) {
                        cs->heritage = emalloc(cs->nb_heritage*sizeof(joinpoint *));
                    } else {
                        cs->heritage = erealloc(cs->heritage, cs->nb_heritage*sizeof(joinpoint *));
                    }
                    cs->heritage[cs->nb_heritage-1] = get_joinpoint_from_ce(ce);
                    ce = ce->parent;
                }

                zend_uint i;
                for (i=0; i<(cs->ce)->num_interfaces; i++) {
                    cs->nb_heritage++;
                    if (cs->nb_heritage==1) {
                        cs->heritage = emalloc(cs->nb_heritage*sizeof(joinpoint *));
                    } else {
                        cs->heritage = erealloc(cs->heritage, cs->nb_heritage*sizeof(joinpoint *));
                    }
                    cs->heritage[cs->nb_heritage-1] = get_joinpoint_from_ce(cs->ce->interfaces[i]);
                }
                #if ZEND_MODULE_API_NO >= 20100525
                //Traits
                for (i=0; i<(cs->ce)->num_traits; i++) {
                    cs->nb_heritage++;
                    if (cs->nb_heritage==1) {
                        cs->heritage = emalloc(cs->nb_heritage*sizeof(joinpoint *));
                    } else {
                        cs->heritage = erealloc(cs->heritage, cs->nb_heritage*sizeof(joinpoint *));
                    }
                    cs->heritage[cs->nb_heritage-1] = get_joinpoint_from_ce(cs->ce->traits[i]);
                }
                #endif
            }
        }
    }
    return cs; 
}

char *get_class_part_with_ns(char *class) {
    int i = strlen(class)-1;
    char *toReturn;
    while (i>=0 && class[i]!='\\') {
        i--;
    }
    if (i<0) {
        toReturn = estrdup(class);
    } else {
        toReturn = estrdup(class+i+1);
    }
    php_strtolower (toReturn, strlen(toReturn));
    return toReturn;
}

int compare_ns_and_class (pointcut *pc, joinpoint *jp) {
    if (!compare_namespace(pc->num_ns, pc->ns, jp->num_ns, jp->ns)) {
        return 0;
    }
    if (!strcmp_with_joker(pc->class_name, jp->class_name)) {
        return 0;
    }
    return 1;
}

int pointcut_match_joinpoint (pointcut *pc, joinpoint *jp) {
    if (pc->static_state!=2 && pc->static_state!=jp->static_state) {
        return 0;
    }
    if (pc->scope != NULL && !(pc->scope & jp->scope)) {
        return 0;
    }
    if (pc->class_name==NULL && !strcmp(pc->method,"*")) {
        return 1;
    }
    if (!strcmp_with_joker(pc->method, jp->method)) {
        return 0;
    }
    if (pc->class_name==NULL && jp->class_name==NULL) {
        return 1;
    }
    if ((pc->class_name==NULL || jp->class_name==NULL) && pc->class_name!=jp->class_name) {
        return 0;
    }
    if (compare_ns_and_class(pc, jp)) {
        return 1;
    }
    int i;
    for (i = 0;i<jp->nb_heritage;i++) {
        if (compare_ns_and_class(pc,jp->heritage[i])){
            return 1;
        }
    }
    return 0;

}


static zval *get_current_args (zend_op_array *ops TSRMLS_DC) {
    void **p;
    int arg_count;
    int i;
    zval *return_value;
    MAKE_STD_ZVAL(return_value);
    array_init(return_value);
    zend_execute_data *ex = EG(current_execute_data);
    if (!ex || !ex->function_state.arguments) {
        zend_error(E_WARNING, "Problem in AOP getArgs");
        return;
    }

    p = ex->function_state.arguments;
    arg_count = (int)(zend_uintptr_t) *p;

    for (i=0; i<arg_count; i++) {
        zval *element;

//        ALLOC_ZVAL(element);
        element = *((zval **) (p-(arg_count-i)));
//        zval_copy_ctor(element);
//        INIT_PZVAL(element);
        zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
    }
    return return_value;

}
