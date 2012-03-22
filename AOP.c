#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "AOP.h"
#include "Zend/zend_operators.h"


ZEND_DECLARE_MODULE_GLOBALS(AOP)




static void php_aop_init_globals(zend_AOP_globals *aop_globals)
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
    {
        NULL, NULL, NULL
    }
};



PHP_MSHUTDOWN_FUNCTION(AOP)
{
    /*	int i;
    	for (i=0;i<AOP_G(count_pcs);i++) {
            	efree(AOP_G(pcs)[i].selector);
    	}
    	efree(AOP_G(pcs));
    */
    return SUCCESS;
}

PHP_RINIT_FUNCTION(AOP)
{
    AOP_G(count_pcs)=0;
    AOP_G(overloaded)=0;
    return SUCCESS;
}

PHP_MINIT_FUNCTION(AOP)
{
    ZEND_INIT_MODULE_GLOBALS(AOP, php_aop_init_globals,NULL);
    REGISTER_LONG_CONSTANT("AOP_PRIVATE", ZEND_ACC_PRIVATE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_PUBLIC", ZEND_ACC_PUBLIC, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("AOP_PROTECTED", ZEND_ACC_PROTECTED, CONST_CS | CONST_PERSISTENT);

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
    AOP_G(pcs)[count].selector = make_class_struct(selector);
    Z_ADDREF_P(callback);
    AOP_G(pcs[count]).callback = callback;
}

ZEND_DLEXPORT void aop_execute (zend_op_array *ops TSRMLS_DC) {
    int i,arounded;
    arounded=0;
    char *func;
    class_struct *class = get_current_class_struct();
    if (class==NULL || class->method==NULL || AOP_G(overloaded)) {
        _zend_execute(ops TSRMLS_CC);
        return;
    }
    ipointcut *previous_ipc;
    previous_ipc = NULL;
    zval *object;
    for (i=0;i<AOP_G(count_pcs);i++) {
        if (compare_class_struct(AOP_G(pcs)[i].selector,class)) {
            arounded=1;

            zval *rtr=NULL;
            MAKE_STD_ZVAL(object);
            Z_TYPE_P(object) = IS_OBJECT;
            (object)->value.obj = AOP_create_handler(AOP_class_entry TSRMLS_CC);
            AOP_object *obj = (AOP_object *)zend_object_store_get_object(object TSRMLS_CC);
            obj->args = get_current_args(ops TSRMLS_CC);
            obj->argsOverload = 0;
            zval_copy_ctor(obj->args);
            if (class->class_name!=NULL) {
                obj->funcName = emalloc(strlen(class->class_name)+strlen(class->method)+3);
                snprintf(obj->funcName, strlen(class->class_name)+strlen(class->method)+3, "%s::%s", class->class_name, class->method);
            } else {
                obj->funcName = class->method;
            }
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
        AOP_G(overloaded)=1;
        zval *ret;
        ret = pointcut_execute(previous_ipc);
        AOP_G(overloaded)=0;
        if (!EG(exception)) {
            if (EG(return_value_ptr_ptr)) {
                *EG(return_value_ptr_ptr)=ret;
            }
        }

    } else {
        _zend_execute(ops TSRMLS_CC);
    }
    if (previous_ipc!=NULL) {
        efree(previous_ipc);
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

        AOP_G(overloaded)=0;
        _zend_execute(EG(active_op_array) TSRMLS_CC);
        AOP_G(overloaded)=1;

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
    zend_class_entry **ce;

    if (zend_lookup_class(str1, strlen(str1), &ce TSRMLS_CC) == FAILURE) {
        return 0;
    }
    zend_uint i;
    for (i=0; i<(*ce)->num_interfaces; i++) {
        if (strcmp_with_joker(str2, (*ce)->interfaces[i]->name)) {
            return 1;
        }
    }
    while ((*ce)) {
         if (strcmp_with_joker(str2, (*ce)->name)) {
             return 1;
         }
         ce = &((*ce)->parent);
    }

    return 0;
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

int get_scope_part_2 (char *partial) {
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
int get_scope_part (char *str) {
    int i=0;
    int toReturn = NULL;
    char *partial = NULL;
    int last = 0;
    while (i<strlen(str)) {
        if (str[i]==' ') {
            partial = estrndup(str,i);
            int temp_return = get_scope_part_2(partial);
            if (temp_return!=NULL) {
                toReturn |= get_scope_part_2(partial);
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

int compare (char *str1, char *str2 TSRMLS_DC) {
    if (!strcmp(str1,"*")) {
        return 1;
    }
    char *class1 = get_class_part(str1);
    char *class2 = get_class_part(str2);
    // No class so simple comp
    if (class1==NULL && class2==NULL) {
        efree(class1);
        efree(class2);
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

int compare_class_struct (class_struct *selector, class_struct *class) {
    if (selector->static_state!=2 && selector->static_state!=class->static_state) {
        return 0;
    }
    if (selector->scope != NULL && !(selector->scope & class->scope)) {
        return 0;
    }
    if (selector->class_name==NULL && !strcmp(selector->method,"*")) {
        return 1;
    }
    if (!strcmp_with_joker(selector->method, class->method)) {
        return 0;
    }
    if (selector->class_name==NULL && class->class_name==NULL) {
        return 1;
    }
    if ((selector->class_name==NULL || class->class_name==NULL) && selector->class_name!=class->class_name) {
        return 0;
    }

    zend_class_entry *ce = class->ce;
    char **ns;
    ns = class->ns;
    int num_ns = class->num_ns;
    while (ce!=NULL) {
         if (strcmp_with_joker(selector->class_name, get_class_part_with_ns(ce->name)) && compare_namespace(selector->num_ns, selector->ns, num_ns, ns)) {
             return 1;
         }
         ce = ce->parent;
         if (ce!=NULL) {
    //        num_ns = get_ns(ce->name, &ns);
         }
    }
    zend_uint i;
    for (i=0; i<(class->ce)->num_interfaces; i++) {
        if (strcmp_with_joker(selector->class_name, (class->ce)->interfaces[i]->name)) {
            return 1;
        }
    }
    #if ZEND_MODULE_API_NO >= 20100525
    //Traits
    for (i=0; i<(class->ce)->num_traits; i++) {
        if (strcmp_with_joker(selector->class_name, (class->ce)->traits[i]->name)) {
            return 1;
        }
    }
    #endif

    return 0;
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

class_struct *make_class_struct(zval *value) {
    class_struct *cs = emalloc (sizeof(class_struct));
    cs->method = NULL;
    cs->class_name = NULL;
    cs->scope = NULL;
    cs->ce = NULL;
    cs->static_state = 2;
    if (Z_TYPE_P(value)==IS_STRING) {
        char *strval;
        strval = estrdup (Z_STRVAL_P(value));
        php_strtolower(strval, strlen(strval));
        cs->class_name = get_class_part(strval);
        cs->scope = get_scope_part(strval);
        cs->static_state = is_static(strval);
        if (cs->class_name!=NULL) {
            cs->method = get_method_part(strval);
            cs->num_ns = get_ns(cs->class_name, &(cs->ns));
            if (cs->num_ns>0) {
                cs->class_name = get_class_part_with_ns(cs->class_name);
            }
        } else {
            cs->method = estrdup(strval);
        }
        
    } else if (Z_TYPE_P(value)==IS_ARRAY) {
        zval **zvalue;
        if (zend_hash_find(Z_ARRVAL_P(value), "class",6,(void **)&zvalue)!=FAILURE) {
            cs->class_name = estrdup(Z_STRVAL_PP(zvalue));
        }

        if (zend_hash_find(Z_ARRVAL_P(value), "namespace",10,(void **)&zvalue)!=FAILURE) {
            cs->num_ns = get_ns_without_class(Z_STRVAL_PP(zvalue), &(cs->ns));
        }

        if (zend_hash_find(Z_ARRVAL_P(value), "method",7,(void **)&zvalue)!=FAILURE) {
            cs->method = estrdup(Z_STRVAL_PP(zvalue));
        }
        
        if (zend_hash_find(Z_LVAL_P(value), "scope",6,(void **)&zvalue)!=FAILURE) {
            cs->scope = Z_LVAL_PP(zvalue);
        }

        if (zend_hash_find(Z_LVAL_P(value), "static",7,(void **)&zvalue)!=FAILURE) {
            cs->static_state = Z_LVAL_PP(zvalue);
        }
    } else {
        zend_error_noreturn(E_ERROR, "Bad parameter for AOP_add selector ");
    }
    return cs;
}

class_struct *get_current_class_struct() {
    class_struct *cs = emalloc (sizeof(class_struct));
    cs->method=NULL;
    cs->class_name = NULL;
    cs->ce = NULL;
    cs->static_state = NULL;

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
            }
        }
    }
    return cs; 
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
