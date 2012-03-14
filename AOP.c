#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "AOP.h"
#include "Zend/zend_operators.h"


ZEND_DECLARE_MODULE_GLOBALS(AOP)



static void
php_aop_init_globals(zend_AOP_globals *aop_globals)
{
}

static zend_function_entry AOP_functions[] =
{
    ZEND_FE(AOP_add, NULL)
    {NULL, NULL, NULL}
};




zend_module_entry AOP_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_AOP_EXTNAME,
    AOP_functions,
    PHP_MINIT(AOP),
    PHP_MSHUTDOWN(AOP),
    PHP_RINIT(AOP),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_AOP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AOP
ZEND_GET_MODULE(AOP)
#endif

static zend_class_entry* AOP_class_entry;

zend_object_handlers AOP_object_handlers;

void AOP_free_storage(void *object TSRMLS_DC)
{
    AOP_object *obj = (AOP_object *)object;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}




zend_object_value AOP_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    AOP_object *obj = (AOP_object *)emalloc(sizeof(AOP_object));
    memset(obj, 0, sizeof(AOP_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

    retval.handle = zend_objects_store_put(obj, NULL,
        AOP_free_storage, NULL TSRMLS_CC);
    retval.handlers = &AOP_object_handlers;

    return retval;
}

PHP_METHOD(AOP, getArgs);
PHP_METHOD(AOP, getThis);
PHP_METHOD(AOP, getFunctionName);
PHP_METHOD(AOP, process);
PHP_METHOD(AOP, processWithArgs);

ZEND_BEGIN_ARG_INFO(arginfo_AOP_getargs, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_AOP_getfunctionname, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_AOP_process, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_AOP_getthis, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_AOP_processwithargs, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry AOP_methods[] = {
        PHP_ME(AOP, getArgs,arginfo_AOP_getargs, 0)
        PHP_ME(AOP, getThis,arginfo_AOP_getthis, 0)
        PHP_ME(AOP, process,arginfo_AOP_process, 0)
        PHP_ME(AOP, processWithArgs,arginfo_AOP_processwithargs, 0)
        PHP_ME(AOP, getFunctionName,arginfo_AOP_getfunctionname, 0)
    {NULL, NULL, NULL}
};



PHP_MSHUTDOWN_FUNCTION(AOP)
{
        return SUCCESS;
}

PHP_RINIT_FUNCTION(AOP)
{
	AOP_G(count_pcs)=0;
        return SUCCESS;
}

PHP_MINIT_FUNCTION(AOP)
{
	ZEND_INIT_MODULE_GLOBALS(AOP, php_aop_init_globals,NULL);

	zend_class_entry ce;

        INIT_CLASS_ENTRY(ce, "AOP", AOP_methods);
        AOP_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

        AOP_class_entry->create_object = AOP_create_handler;
        memcpy(&AOP_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
        AOP_object_handlers.clone_obj = NULL;


	_zend_execute = zend_execute;
	zend_execute  = aop_execute;
        return SUCCESS;
}
PHP_METHOD(AOP, process)
{
    zval *toReturn;
    AOP_object *obj = (AOP_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    toReturn = exec(obj, NULL TSRMLS_CC);
    if (toReturn != NULL) {
            COPY_PZVAL_TO_ZVAL(*return_value, toReturn);
    }
}

PHP_METHOD(AOP, getArgs)
{
    AOP_object *obj = (AOP_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    Z_TYPE_P(return_value)   = IS_ARRAY;
    Z_ARRVAL_P(return_value) = Z_ARRVAL_P(obj->args);
        Z_ADDREF_P(return_value);
    return;
}
PHP_METHOD(AOP, getThis)
{
    AOP_object *obj = (AOP_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->this!=NULL) {
        Z_TYPE_P(return_value)   = IS_OBJECT;
        Z_OBJVAL_P(return_value) = Z_OBJVAL_P(obj->this);
        //In order not to destroy OBJVAL(obj->this) 
        //Need to verify memory leak
        Z_ADDREF_P(return_value);
    }
    return;
}

PHP_METHOD(AOP, getFunctionName)
{
    AOP_object *obj = (AOP_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    Z_TYPE_P(return_value)   = IS_STRING;
    Z_STRVAL_P(return_value) = estrdup(obj->funcName);
    Z_STRLEN_P(return_value) = strlen(obj->funcName);
    return;
}

PHP_METHOD(AOP, processWithArgs)
{
    AOP_object *obj = (AOP_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
        zend_error(E_ERROR, "Problem in processWithArgs");
    }

    zval *toReturn;
    toReturn = exec(obj, params TSRMLS_CC);
    if (toReturn != NULL) {
            COPY_PZVAL_TO_ZVAL(*return_value, toReturn);
    }


}

ZEND_FUNCTION(AOP_add)
{
	zval *callback;
	zval *selector;
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &selector, &callback) == FAILURE) {
                return;
        }
//	return;
	AOP_G(count_pcs)++;
	if (AOP_G(count_pcs)==1) {
		AOP_G(pcs) = emalloc(sizeof(pointcut));
	} else {
		AOP_G(pcs) = erealloc(AOP_G(pcs),AOP_G(count_pcs)*sizeof(pointcut));
	}
	int count=AOP_G(count_pcs)-1;
	AOP_G(pcs)[count].selector = estrdup(Z_STRVAL_P(selector));
	Z_ADDREF_P(callback);
	AOP_G(pcs[count]).callback = callback;
}

ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC) {
	int i,arounded;
	arounded=0;
	char *func;
	func = get_function_name(ops TSRMLS_CC);
	if (func==NULL) {
		_zend_execute(ops TSRMLS_CC);
		return;
	}
	ipointcut *previous_ipc;
	previous_ipc = NULL;
        zval *object;
	for (i=0;i<AOP_G(count_pcs);i++) {
		if (compare(AOP_G(pcs)[i].selector,func TSRMLS_CC)) {
           arounded=1;
            
            zval *rtr=NULL;
            MAKE_STD_ZVAL(object);
            Z_TYPE_P(object) = IS_OBJECT;
            (object)->value.obj = AOP_create_handler(AOP_class_entry TSRMLS_CC);
            AOP_object *obj = (AOP_object *)zend_object_store_get_object(object TSRMLS_CC);
            obj->args = get_current_args(ops TSRMLS_CC);
            obj->argsOverload = 0;
            zval_copy_ctor(obj->args);
            obj->funcName = estrdup(func);
            if (previous_ipc!=NULL) {
                obj->ipointcut = previous_ipc;
            } else {
                obj->op = ops;
                obj->ex = EG(current_execute_data);
                obj->this = EG(This);
		obj->scope = EG(scope);
            }
            previous_ipc = emalloc(sizeof(ipointcut));
            previous_ipc->callback = AOP_G(pcs)[i].callback;
            previous_ipc->object = object;
		}
	}
	if (arounded) {
		zval *ret;
		ret = pointcut_execute(previous_ipc);
		if (!EG(exception)) {
	                if (EG(return_value_ptr_ptr)) {
        	        	*EG(return_value_ptr_ptr)=ret;
	        	}
        	}

	} else {
		_zend_execute(ops TSRMLS_CC);
	}
}

static char *get_function_name(zend_op_array *ops TSRMLS_DC) {
	zend_execute_data *data;
	char              *func = NULL;
	const char        *cls = NULL;
	char              *ret = NULL;
	int                len;
	zend_function      *curr_func;

	data = EG(current_execute_data);

	if (data) {
		curr_func = data->function_state.function;

		func = curr_func->common.function_name;

		if (func) {
			if (data->object) {
				cls = Z_OBJCE(*data->object)->name;
			}
 		

      			if (cls) {
			        len = strlen(cls) + strlen(func) + 10;
			        ret = (char*)emalloc(len);
			        snprintf(ret, len, "%s::%s", cls, func);
		        } else {
			        ret = estrdup(func);
      			}
		}
	}
	return ret;
}

/*int compare (char *str1, char *str2) {
	return !strcmp(str1,str2);
}*/

zval *pointcut_execute (ipointcut *pc) {
    zval *args[1];
    args[0] = (zval *)&(pc->object);
    zval *zret_ptr=NULL;
        zend_fcall_info fci;
        zend_fcall_info_cache fcic= { 0, NULL, NULL, NULL, NULL };
        TSRMLS_FETCH();
        fci.param_count= 1;
        fci.size= sizeof(fci);
        fci.function_table= EG(function_table);
        fci.function_name= pc->callback;
        fci.symbol_table= NULL;
        fci.retval_ptr_ptr= &zret_ptr;
        fci.params = (zval ***)args;
        fci.object_ptr= NULL;
        fci.no_separation= 0;
        if (zend_call_function(&fci, &fcic TSRMLS_CC) == FAILURE) {
        //php_printf("BUG\n");
        }
    if (!EG(exception)) {
        //php_printf("AFTER EXECUTE\n");
        return zret_ptr;
    }
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
                zend_error(E_WARNING, "ooops");
                return;
        }

        p = ex->function_state.arguments;
        arg_count = (int)(zend_uintptr_t) *p;           /* this is the amount of arguments passed to func_get_args(); */

        for (i=0; i<arg_count; i++) {
                zval *element;

                ALLOC_ZVAL(element);
                *element = **((zval **) (p-(arg_count-i)));
                zval_copy_ctor(element);
                INIT_PZVAL(element);
                zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
        }
    return return_value;

}

zval *exec(AOP_object *obj, zval *args TSRMLS_DC) {
    if (args!=NULL) {
        obj->argsOverload=1;
        obj->args = args;
    }
    if (obj->ipointcut==NULL) {
        zend_execute_data *prev_data;
        zval **original_return_value;   
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
        
        EG(active_op_array) = (zend_op_array *) obj->op;
        EG(current_execute_data) = obj->ex;
        EG(This) = obj->this;
        EG(scope) = obj->scope;
        if (obj->argsOverload) {
            int i,arg_count;
            zend_execute_data *ex = EG(current_execute_data);
            arg_count = (int)(zend_uintptr_t) *(ex->function_state.arguments);
            HashPosition pos;
            i=0;
            zval ** temp=NULL;
            zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(obj->args), &pos);

            while (zend_hash_get_current_data_ex(Z_ARRVAL_P(obj->args), (void **)&temp, &pos)==SUCCESS) {
               i++;
               zend_vm_stack_push_nocheck(*temp TSRMLS_CC);
               zend_hash_move_forward_ex(Z_ARRVAL_P(obj->args), &pos);
            }
            ex->function_state.arguments = zend_vm_stack_top(TSRMLS_C);
            zend_vm_stack_push_nocheck((void*)(zend_uintptr_t)i TSRMLS_CC);
        }

        _zend_execute(EG(active_op_array) TSRMLS_CC);
        
        //Take previous context
        EG(This) = prev_this;
        EG(opline_ptr) = original_opline_ptr;
        EG(active_op_array) = (zend_op_array *) prev_op;
        EG(current_execute_data) = prev_data;
        EG(active_symbol_table) = calling_symbol_table;
	EG(scope)=current_scope;
// zend_throw_exception(zend_exception_get_default(TSRMLS_C), "String could not be parsed as XML", 0 TSRMLS_CC);
//return;
        //Only if we do not have exception
        if (!EG(exception)) {
           return (zval *)*EG(return_value_ptr_ptr);
        } else {


        }
        
    } else {
        zval *exec_return;
        //php_printf("BEFORE EXE");
        if (obj->argsOverload) {
            AOP_object *objN = (AOP_object *)zend_object_store_get_object(obj->ipointcut->object TSRMLS_CC);
            objN->argsOverload=1;
            objN->args = obj->args;
            zval_copy_ctor(objN->args);
        }
        exec_return = pointcut_execute(obj->ipointcut);
        //php_printf("AFTER EXE");

        //Only if we do not have exception
        if (!EG(exception)) {
            return exec_return;
        } 
    }

    return NULL;

}

int instance_of (char *str1, char *str2 TSRMLS_DC) {
        zend_class_entry **ce1;
        zend_class_entry **ce2;

     if (zend_lookup_class(str1, strlen(str1), &ce1 TSRMLS_CC) == FAILURE) {
        return 0;
     }
     if (EG(class_table)==NULL) {
	return 0;
     }
     php_strtolower(str2, strlen(str2));
     if (zend_hash_find(EG(class_table), str2, strlen(str2)+1, (void **) &ce2)) {
        return 0;
     }
    return instanceof_function(*ce1, *ce2 TSRMLS_CC);
}

char* get_class_part (char *str) {
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

int strcmp_with_joker (char *str_with_jok, char *str) {
    int i;
    int joker=0;
    for (i=0;i<strlen(str_with_jok);i++) {
        if (str_with_jok[i]=='*') {
            joker=i;
            break;
        }
    }
    if (joker) {
        return !strcmp(estrndup(str_with_jok,joker),estrndup(str,joker));
    } else {
        return !strcmp(str_with_jok,str);
    }

}

int compare (char *str1, char *str2 TSRMLS_DC) {
    char *class1 = get_class_part(str1);
    char *class2 = get_class_part(str2);
    // No class so simple comp
    if (class1==NULL && class2==NULL) {
        return strcmp_with_joker(str1,str2);
    }
    // Only one with class => false
    if ((class1!=NULL && class2==NULL) || (class1==NULL && class2!=NULL)) {
        return 0;
    }

    //Two different classes => false
    if (class1!=NULL && class2!=NULL && !instance_of(class2,class1 TSRMLS_CC)) {
        return 0;
    }
    //Method only joker
    if (!strcmp (get_method_part(str1), "*")) {
        return 1;
    }
    return strcmp_with_joker(get_method_part(str1),get_method_part(str2));
}
        
