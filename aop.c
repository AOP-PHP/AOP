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
#include "Lexer.h"
#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

ZEND_DECLARE_MODULE_GLOBALS(aop)

#define DEBUG_OBJECT_HANDLERS 1

void (*aop_old_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
void aop_execute_ex(zend_execute_data *execute_data TSRMLS_DC);

void (*aop_old_execute_internal)(zend_execute_data *current_execute_data, zval *return_value);
void aop_execute_internal(zend_execute_data *current_execute_data, zval *return_value);

static void php_aop_init_globals(zend_aop_globals *aop_globals)
{
    zend_hash_init(&aop_globals->pointcuts, 0, NULL, zval_pointcut_dtor, 1);
}

static pointcut * alloc_pointcut () {
    pointcut *pc = (pointcut *)emalloc(sizeof(pointcut));
    pc->scope = 0;
    pc->static_state = 2;
    pc->method_jok = 0;
    pc->class_jok = 0;
    pc->class_name = NULL;
    pc->method = NULL;
    pc->selector = NULL;
    pc->kind_of_advice = 0;
    pc->re_method = NULL;
    pc->re_class = NULL;
    return pc;
}

static aop_func_info_dtor(aop_func_info *info) {
	if (!Z_ISUNDEF(info->obj)) {
		zval_ptr_dtor(&info->obj);
	}
	if (!Z_ISUNDEF(info->closure)) {
		zval_ptr_dtor(&info->closure);
	}
    zend_string_release(info->funcname);
    
	efree(info);
}

static pointcut_dtor(pointcut *pc) {
    if (pc->class_name != NULL) {
        zend_string_release(pc->class_name);
    }
    if (pc->method != NULL) {
        zend_string_release(pc->method);
    }
    if (pc->selector != NULL) {
        zend_string_release(pc->selector);
    }
    if (pc->function_info!=NULL) {
        aop_func_info_dtor(pc->function_info);
    }
    efree(pc);

}

static zval_pointcut_dtor(zval *zv) {
    pointcut_dtor(Z_PTR_P(zv));
}

char *str_replace(const char *str, const char *from, const char *to) {

	/* Adjust each of the below values to suit your needs. */

	/* Increment positions cache size initially by this number. */
	size_t cache_sz_inc = 16;
	/* Thereafter, each time capacity needs to be increased,
	 * multiply the increment by this factor. */
	const size_t cache_sz_inc_factor = 3;
	/* But never increment capacity by more than this number. */
	const size_t cache_sz_inc_max = 1048576;

	char *pret, *ret = NULL;
	const char *pstr2, *pstr = str;
	size_t i, count = 0;
	ptrdiff_t *pos_cache = NULL;
	size_t cache_sz = 0;
	size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

	/* Find all matches and cache their positions. */
	while ((pstr2 = strstr(pstr, from)) != NULL) {
		count++;

		/* Increase the cache size when necessary. */
		if (cache_sz < count) {
			cache_sz += cache_sz_inc;
			pos_cache = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
			if (pos_cache == NULL) {
				goto end_repl_str;
			}
			cache_sz_inc *= cache_sz_inc_factor;
			if (cache_sz_inc > cache_sz_inc_max) {
				cache_sz_inc = cache_sz_inc_max;
			}
		}

		pos_cache[count-1] = pstr2 - str;
		pstr = pstr2 + fromlen;
	}

	orglen = pstr - str + strlen(pstr);

	/* Allocate memory for the post-replacement string. */
	if (count > 0) {
		tolen = strlen(to);
		retlen = orglen + (tolen - fromlen) * count;
	} else	retlen = orglen;
	ret = malloc(retlen + 1);
	if (ret == NULL) {
		goto end_repl_str;
	}

	if (count == 0) {
		/* If no matches, then just duplicate the string. */
		strcpy(ret, str);
	} else {
		/* Otherwise, duplicate the string whilst performing
		 * the replacements using the position cache. */
		pret = ret;
		memcpy(pret, str, pos_cache[0]);
		pret += pos_cache[0];
		for (i = 0; i < count; i++) {
			memcpy(pret, to, tolen);
			pret += tolen;
			pstr = str + pos_cache[i] + fromlen;
			cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - fromlen;
			memcpy(pret, pstr, cpylen);
			pret += cpylen;
		}
		ret[retlen] = '\0';
	}

end_repl_str:
	/* Free the cache and return the post-replacement string,
	 * which will be NULL in the event of an error. */
	free(pos_cache);
	return ret;
}

static int pointcut_match_zend_class_entry (pointcut *pc, zend_class_entry *ce) {
    int i, matches;
    if (ce == NULL) {
        return 0;
    }
    matches = pcre_exec(pc->re_class, NULL, ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), 0, 0, NULL, 0);
    if (matches >= 0) {
        return 1;
    }
    for (i = 0; i < (int) ce->num_interfaces; i++) {
        matches = pcre_exec(pc->re_class, NULL, ZSTR_VAL(ce->interfaces[i]->name), ZSTR_LEN(ce->interfaces[i]->name), 0, 0, NULL, 0);
        if (matches >= 0) {
            return 1;
        }
    }
#if ZEND_MODULE_API_NO >= 20100525
    for (i = 0; i < (int) ce->num_traits; i++) {
        matches = pcre_exec(pc->re_class, NULL, ZSTR_VAL(ce->traits[i]->name), ZSTR_LEN(ce->traits[i]->name), 0, 0, NULL, 0);
        if (matches>=0) {
            return 1;
        }
    }
#endif
    ce = ce->parent;
    while (ce != NULL) {
        matches = pcre_exec(pc->re_class, NULL, ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), 0, 0, NULL, 0);
        if (matches >= 0) {
            return 1;
        }
        ce = ce->parent;
    }
    return 0;
}

static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func) {
//static int pointcut_match_zend_function (pointcut *pc, zend_function *curr_func, zend_execute_data *data) {
    if (curr_func->common.fn_flags & ZEND_ACC_CLOSURE) {
        return 0;
    }
    int comp_start = 0;
    TSRMLS_FETCH();
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
    if (pc->class_name == NULL && ZSTR_VAL(pc->method)[0] == '*' && ZSTR_VAL(pc->method)[1]=='\0') {
        return 1;
    }
    if (pc->class_name == NULL && curr_func->common.scope != NULL) {    
        return 0;
    }

    if (pc->method_jok) {
        int matches = pcre_exec(pc->re_method, NULL, ZSTR_VAL(curr_func->common.function_name), ZSTR_LEN(curr_func->common.function_name), 0, 0, NULL, 0);

        if (matches < 0) {
            return 0;
        }
    } else {
        if (ZSTR_VAL(pc->method)[0]=='\\') {
            comp_start=1;
        }

        if (strcasecmp(ZSTR_VAL(pc->method)+comp_start, ZSTR_VAL(curr_func->common.function_name))) {
            return 0;
        }
    }
    return 1;
}

void make_regexp_on_pointcut (pointcut **pc) { 
    pcre_extra *pcre_extra = NULL;
    int preg_options = 0;
    char *regexp;
    char *regexp_buffer;
    zend_string *tempregexp;
    TSRMLS_FETCH();
    (*pc)->method_jok = (strchr(ZSTR_VAL((*pc)->method), '*') != NULL);
    regexp = (ZSTR_VAL((*pc)->method));
    regexp_buffer = str_replace(regexp, "**\\", "[.#}");
    //efree(regexp);
    regexp = regexp_buffer;
    regexp_buffer = str_replace(regexp, "**", "[.#]");
    //efree(regexp);
    regexp = regexp_buffer;
    regexp_buffer = str_replace(regexp, "\\", "\\\\");
    //efree(regexp);
    regexp = regexp_buffer;
    regexp_buffer = str_replace(regexp, "*", "[^\\\\]*");
    //efree(regexp);
    regexp = regexp_buffer;
    regexp_buffer = str_replace(regexp, "[.#]", ".*");
    //efree(regexp);
    regexp = regexp_buffer;
    regexp_buffer = str_replace(regexp, "[.#}", "(.*\\\\)?");
    //efree(regexp);
    regexp = regexp_buffer;
    if (regexp[0]!='\\') {
		tempregexp = strpprintf(500,  "/^%s$/i", regexp);
    } else {
		tempregexp = strpprintf(500,  "/^%s$/i", regexp+2);
    }
    //efree(regexp);
    (*pc)->re_method = pcre_get_compiled_regex(tempregexp, &pcre_extra, &preg_options TSRMLS_CC);
    efree(tempregexp);
    if (!(*pc)->re_method) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid expression");
    }
    if ((*pc)->class_name != NULL) {
        regexp = ZSTR_VAL((*pc)->class_name);
        regexp_buffer = str_replace(regexp, "**\\", "[.#}");
        //efree(regexp);
        regexp = regexp_buffer;
        regexp_buffer = str_replace(regexp, "**", "[.#]");
        //efree(regexp);
        regexp = regexp_buffer;
        regexp_buffer = str_replace(regexp, "\\", "\\\\");
        //efree(regexp);
        regexp = regexp_buffer;
        regexp_buffer = str_replace(regexp, "*", "[^\\\\]*");
        //efree(regexp);
        regexp = regexp_buffer;
        regexp_buffer = str_replace(regexp, "[.#]", ".*");
        //efree(regexp);
        regexp = regexp_buffer;
        regexp_buffer = str_replace(regexp, "[.#}", "(.*\\\\)?");
        //efree(regexp);
        regexp = regexp_buffer;
		if (regexp[0]!='\\') {
			tempregexp = strpprintf(500,  "/^%s$/i", regexp);
		} else {
			tempregexp = strpprintf(500,  "/^%s$/i", regexp+2);
		}
        //efree(regexp);
        (*pc)->re_class = pcre_get_compiled_regex(tempregexp, &pcre_extra, &preg_options TSRMLS_CC);
        efree(tempregexp);
        if (!(*pc)->re_class) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid expression");
        }
    }
}


static void add_pointcut (aop_func_info *function_info, zend_string *selector, int type TSRMLS_DC) {
    pointcut *pc = NULL;
	scanner_state *state = (scanner_state *)emalloc(sizeof(scanner_state)); 
    scanner_token *token = (scanner_token *)emalloc(sizeof(scanner_token));

    if (ZSTR_LEN(selector) < 2) {
        zend_error(E_ERROR, "The given pointcut is invalid. You must specify a function call, a method call or a property operation"); 
    }

    char *temp_str = NULL;
    int is_class = 0;
    

    pc = alloc_pointcut();
    pc->function_info = function_info;

    pc->selector = selector;
    pc->kind_of_advice = type;
    pc->selector = selector;
    pc->function_info = function_info;
    char *selector_str = ZSTR_VAL(selector);
    state->start = selector_str;
    state->end = state->start;
    while(0 <= scan(state, token)) {
        switch (token->TOKEN) {
            case TOKEN_STATIC:
                pc->static_state=token->int_val;
                break;
            case TOKEN_SCOPE:
                pc->scope |= token->int_val;
                break;
            case TOKEN_CLASS:
                pc->class_name = zend_string_init(temp_str, strlen(temp_str), 0);;
                efree(temp_str);
                temp_str=NULL;
                is_class=1;
                break;
            case TOKEN_FUNCTION:
                if (is_class) {
                    pc->kind_of_advice |= AOP_KIND_METHOD;
                } else {
                    pc->kind_of_advice |= AOP_KIND_FUNCTION;
                }
                break;
            case TOKEN_TEXT:
                if (temp_str!=NULL) {
                    efree(temp_str);
                }
                temp_str=estrdup(token->str_val);
                efree(token->str_val);
                break;
            default:
                break;
        }
    }
    pc->method = zend_string_init(temp_str, strlen(temp_str), 0);;
    efree(temp_str);
    
    efree(state);
    efree(token);

    make_regexp_on_pointcut(&pc);
    zend_hash_next_index_insert_ptr(&aop_g(pointcuts), pc);

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
    zend_hash_destroy(&aop_g(pointcuts));
    return SUCCESS;
}


PHP_RINIT_FUNCTION(aop)
{
    aop_g(overloaded) = 0;
    return SUCCESS;
}

PHP_INI_BEGIN()
    STD_PHP_INI_BOOLEAN("aop.enable","1",PHP_INI_ALL, OnUpdateBool, aop_enable, zend_aop_globals, aop_globals)
PHP_INI_END()


static zend_object_handlers aop_joinpoint_object_handlers;

aop_joinpoint_object * php_aop_joinpoint_object_fetch_object(zend_object *obj) {
      return (aop_joinpoint_object *)((char *)obj - XtOffsetOf(aop_joinpoint_object, std));
}

static void aop_joinpoint_object_free_storage(zend_object *object TSRMLS_DC)
{
    aop_joinpoint_object *jp_object =  php_aop_joinpoint_object_fetch_object(object);
    if (jp_object->funcname!=NULL) {
        zend_string_release(jp_object->funcname);
    }
	zend_object_std_dtor(&jp_object->std);
}


zend_object * aop_joinpoint_object_new(zend_class_entry *ce TSRMLS_DC) {
     //Allocate sizeof(custom) + sizeof(properties table requirements)
     aop_joinpoint_object *intern = ecalloc(1, 
         sizeof(aop_joinpoint_object) + 
         zend_object_properties_size(ce));
     intern->funcname = NULL;
     // Allocating:
     // struct custom_object {
     //    void *custom_data;
     //    zend_object std;
     // }
     // zval[ce->default_properties_count-1]
     zend_object_std_init(&intern->std, ce TSRMLS_CC);
     aop_joinpoint_object_handlers.offset = XtOffsetOf(aop_joinpoint_object, std);
     aop_joinpoint_object_handlers.free_obj = aop_joinpoint_object_free_storage;
 
     intern->std.handlers = &aop_joinpoint_object_handlers;
 
     return &intern->std;
}
 
// Fetching the custom object:
 
 
 

static zend_class_entry* aop_class_entry;
static zend_class_entry* aop_const_class_entry;

PHP_MINIT_FUNCTION(aop)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "AopJoinpoint", aop_methods);
    aop_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
    aop_class_entry->create_object = aop_joinpoint_object_new;
    ZEND_INIT_MODULE_GLOBALS(aop, php_aop_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    memcpy(&aop_joinpoint_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	aop_joinpoint_object_handlers.offset = XtOffsetOf(aop_joinpoint_object, std);

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

aop_func_info * make_aop_func_info_from_execute_data(zend_execute_data *execute_data) {
    aop_func_info *to_return_ptr = emalloc(sizeof(aop_func_info));
    to_return_ptr->func_ptr = execute_data->func;
    ZVAL_UNDEF(&to_return_ptr->obj);
    ZVAL_UNDEF(&to_return_ptr->closure);

    if (execute_data->func->common.scope != NULL) {
        to_return_ptr->obj = execute_data->This;

        if (!Z_ISUNDEF(to_return_ptr->obj) && Z_REFCOUNTED(to_return_ptr->obj) && Z_COUNTED(to_return_ptr->obj)) {
            Z_ADDREF(to_return_ptr->obj);
        }
        to_return_ptr->ce = execute_data->called_scope; 
    }
    to_return_ptr->funcname = zend_string_copy(execute_data->func->common.function_name);
}

void test_pointcut_and_execute(Bucket *p, zend_execute_data *execute_data, aop_func_info *original, zval *retval_ptr TSRMLS_DC) {
    if (p==NULL) {
		p = aop_g(pointcuts).arData; 
    }
    Bucket *end =  aop_g(pointcuts).arData + aop_g(pointcuts).nNumUsed; 
    if (p>=end) {
        if (original==NULL) {
            aop_old_execute_ex(execute_data TSRMLS_CC);
        } else {
            zval retval;
            zval arg;
            retval_ptr = zend_call_method(Z_ISUNDEF(original->obj)? NULL : &(original->obj), original->ce, &(original->func_ptr), ZSTR_VAL(original->funcname), ZSTR_LEN(original->funcname), retval_ptr, 0, &arg, NULL);
        }
        return;
    }

    zval *_z = &p->val;
    if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) {
        test_pointcut_and_execute(p+1, execute_data, NULL, retval_ptr TSRMLS_CC);
        return;
    }
    pointcut *current_pointcut = Z_PTR_P(_z);
    zval retval;

    if (
            (pointcut_match_zend_function(current_pointcut, execute_data->func) && current_pointcut->kind_of_advice & AOP_KIND_FUNCTION)
            || (pointcut_match_zend_function(current_pointcut, execute_data->func) && current_pointcut->kind_of_advice & AOP_KIND_METHOD && pointcut_match_zend_class_entry(current_pointcut, execute_data->called_scope ) ) 
       ) {
        if (current_pointcut->kind_of_advice & AOP_KIND_AFTER) {
            test_pointcut_and_execute(p+1, execute_data, NULL, retval_ptr TSRMLS_CC);
        }
        zval arg;
        ZVAL_OBJ(&arg, aop_joinpoint_object_new(aop_class_entry TSRMLS_CC));
        aop_joinpoint_object *jp_object = Z_AOP_JOINPOINT_OBJ_P(&arg);
        jp_object->funcname = zend_string_copy(execute_data->func->common.function_name);
        jp_object->current_execute_data = execute_data;
        jp_object->current_pointcut = current_pointcut;
        jp_object->original = make_aop_func_info_from_execute_data(execute_data);
        jp_object->current_pointcut_zval_ptr = p;
        jp_object->execute_data = execute_data;
        jp_object->retval_ptr = retval_ptr;
        zend_call_method(Z_ISUNDEF(current_pointcut->function_info->obj)? NULL : &(current_pointcut->function_info->obj), current_pointcut->function_info->ce, &(current_pointcut->function_info->func_ptr), ZSTR_VAL(current_pointcut->function_info->funcname), ZSTR_LEN(current_pointcut->function_info->funcname), retval_ptr, 1, &arg, NULL);
        aop_func_info_dtor(jp_object->original);

        zval_dtor(&arg);
        if (current_pointcut->kind_of_advice & AOP_KIND_BEFORE) {
            test_pointcut_and_execute(p+1, execute_data, NULL, retval_ptr TSRMLS_CC);
        }
    } else {
        test_pointcut_and_execute(p+1, execute_data, NULL, retval_ptr TSRMLS_CC);
    }
}

void aop_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
    if (aop_g(overloaded)==1) {
        aop_g(overloaded) = 0;
        aop_old_execute_ex(execute_data TSRMLS_CC);
        aop_g(overloaded) = 1;
        return;
    }
    if (!aop_g(aop_enable)) {
        aop_old_execute_ex(execute_data TSRMLS_CC);
        return;
    }
    if (execute_data->func && execute_data->func->common.function_name) {
        test_pointcut_and_execute(NULL, execute_data, NULL, execute_data->return_value TSRMLS_CC);
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

aop_func_info *make_function_info(zend_execute_data *execute_data, zval *zcallable)
{
    zend_string *func_name;
	char *error = NULL;
	zend_string *lc_name;
	zend_bool do_throw = 1;
	zend_bool prepend  = 0;
	zend_object *obj_ptr;
	zend_fcall_info_cache fcc;

    aop_func_info *to_return_ptr = emalloc(sizeof(aop_func_info));
    

    if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_STRICT, &func_name, &fcc, &error)) {
        to_return_ptr->ce = fcc.calling_scope;
        to_return_ptr->func_ptr = fcc.function_handler;
        obj_ptr = fcc.object;
        if (Z_TYPE_P(zcallable) == IS_ARRAY) {
            if (!obj_ptr && to_return_ptr->func_ptr && !(to_return_ptr->func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
                if (do_throw) {
                    zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array specifies a non static method but no object (%s)", error);
                }
                if (error) {
                    efree(error);
                }
                zend_string_release(func_name);
                return NULL;
            } else if (do_throw) {
                zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array does not specify %s %smethod (%s)", to_return_ptr->func_ptr ? "a callable" : "an existing", !obj_ptr ? "static " : "", error);
            }
            if (error) {
                efree(error);
            }
            zend_string_release(func_name);
            return NULL;
        } else if (Z_TYPE_P(zcallable) == IS_STRING) {
            if (do_throw) {
                zend_throw_exception_ex(spl_ce_LogicException, 0, "Function '%s' not %s (%s)", ZSTR_VAL(func_name), to_return_ptr->func_ptr ? "callable" : "found", error);
            }
            if (error) {
                efree(error);
            }
            zend_string_release(func_name);
            return NULL;
        } else {
            if (do_throw) {
                zend_throw_exception_ex(spl_ce_LogicException, 0, "Illegal value passed (%s)", error);
            }
            if (error) {
                efree(error);
            }
            zend_string_release(func_name);
            return NULL;
        }
    } 
    to_return_ptr->funcname = zend_string_copy(func_name);
    to_return_ptr->ce = fcc.calling_scope;
    to_return_ptr->func_ptr = fcc.function_handler;
    obj_ptr = fcc.object;
    if (error) {
        efree(error);
    }

    if (Z_TYPE_P(zcallable) == IS_OBJECT) {
        ZVAL_COPY(&to_return_ptr->closure, zcallable);
    } else {
        ZVAL_UNDEF(&to_return_ptr->closure);
    }
    zend_string_release(func_name);

    if (obj_ptr && !(to_return_ptr->func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
        /* add object id to the hash to ensure uniqueness, for more reference look at bug #40091 */
        ZVAL_OBJ(&to_return_ptr->obj, obj_ptr);
        Z_ADDREF(to_return_ptr->obj);
    } else {
        ZVAL_UNDEF(&to_return_ptr->obj);
    }
    return to_return_ptr;
}



PHP_FUNCTION(aop_add_around)
{	
    zend_string *selector;
	zval *zcallable = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_THROW, ZEND_NUM_ARGS(), "Sz", &selector, &zcallable) == FAILURE) {
		return;
	}
    add_pointcut(make_function_info(execute_data, zcallable), selector, AOP_KIND_AROUND TSRMLS_CC);

}

PHP_FUNCTION(aop_add_before)
{
    zend_string *selector;
	zval *zcallable = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_THROW, ZEND_NUM_ARGS(), "Sz", &selector, &zcallable) == FAILURE) {
		return;
	}

    add_pointcut(make_function_info(execute_data, zcallable), selector, AOP_KIND_BEFORE TSRMLS_CC);
}

PHP_FUNCTION(aop_add_after_throwing)
{
    zend_string *selector;
	zval *zcallable = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_THROW, ZEND_NUM_ARGS(), "Sz", &selector, &zcallable) == FAILURE) {
		return;
	}

    add_pointcut(make_function_info(execute_data, zcallable), selector, AOP_KIND_AFTER|AOP_KIND_CATCH TSRMLS_CC);

}

PHP_FUNCTION(aop_add_after_returning)
{
    zend_string *selector;
	zval *zcallable = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_THROW, ZEND_NUM_ARGS(), "Sz", &selector, &zcallable) == FAILURE) {
		return;
	}

    add_pointcut(make_function_info(execute_data, zcallable), selector, AOP_KIND_AFTER|AOP_KIND_RETURN TSRMLS_CC);
}

PHP_FUNCTION(aop_add_after)
{
    zend_string *selector;
	zval *zcallable = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_THROW, ZEND_NUM_ARGS(), "Sz", &selector, &zcallable) == FAILURE) {
		return;
	}

    add_pointcut(make_function_info(execute_data, zcallable), selector, AOP_KIND_AFTER|AOP_KIND_CATCH|AOP_KIND_RETURN TSRMLS_CC);
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


