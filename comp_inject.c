/* comp_inject extension for PHP (c) 2021 kompadre@gmail.com */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_comp_inject.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_op_array *ci_compile_file(zend_file_handle *file_handle, int type);
zend_op_array *(*original_compile_file)(zend_file_handle *file_handle, int type);


ZEND_BEGIN_MODULE_GLOBALS(comp_inject)
    unsigned char started;
    unsigned char compiling;
    //zend_string *inject_start_string;
    //zend_string *inject_end_string;
ZEND_END_MODULE_GLOBALS(comp_inject)


unsigned char started;
unsigned char compiling;
zend_string *inject_start_string;
zend_string *inject_end_string;

ZEND_DECLARE_MODULE_GLOBALS(comp_inject)


#ifdef ZTS
#define COMPI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(comp_inject, v)
#else
#define COMPI_G(v) (comp_inject_globals.v)
#endif

zend_op_array *ci_compile_file(zend_file_handle *file_handle, int type) /* {{{ */
{
    if(!started || (type != ZEND_INCLUDE && type != ZEND_REQUIRE)) {
        return compile_file(file_handle, type);
    }

    int filenamelen = strlen(file_handle->filename);
    if (NULL != inject_start_string && filenamelen > 0) {
        php_printf(ZSTR_VAL(inject_start_string), file_handle->filename);
    }
    if (NULL == inject_end_string || !filenamelen) {
        return compile_file(file_handle, type);
    }




    /*

        EG(no_extensions)=1;

        new_op_array->scope = zend_get_executed_scope();

        zend_try {
            ZVAL_UNDEF(&local_retval);
            zend_execute(new_op_array, &local_retval);
        } zend_catch {
            destroy_op_array(new_op_array);
            efree_size(new_op_array, sizeof(zend_op_array));
            zend_bailout();
        } zend_end_try();

        if (Z_TYPE(local_retval) != IS_UNDEF) {
            if (retval_ptr) {
                ZVAL_COPY_VALUE(retval_ptr, &local_retval);
            } else {
                zval_ptr_dtor(&local_retval);
            }
        } else {
            if (retval_ptr) {
                ZVAL_NULL(retval_ptr);
            }
        }

        EG(no_extensions)=0;
        zend_destroy_static_vars(new_op_array);
        destroy_op_array(new_op_array);
        efree_size(new_op_array, sizeof(zend_op_array));
        retval = SUCCESS;


    */



    zend_op_array *op_array;
    zval result;
    op_array = compile_file(file_handle, type);
    if (op_array) {
        EG(no_extensions)=1;
        op_array->scope = zend_get_executed_scope();

        zend_try {
            ZVAL_UNDEF(&result);
            zend_execute(op_array, &result);
        } zend_catch {
            destroy_op_array(op_array);
            efree_size(op_array, sizeof(zend_op_array));
            zend_bailout();
        } zend_end_try();

        if (Z_TYPE(result) != IS_UNDEF) {
            zval_ptr_dtor(&result);
        }

        EG(no_extensions)=0;

        if (ZEND_MAP_PTR(op_array->static_variables_ptr)) {
            HashTable *ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
            if (ht) {
                zend_array_destroy(ht);
                ZEND_MAP_PTR_SET(op_array->static_variables_ptr, NULL);
            }
        }

        destroy_op_array(op_array);
        efree_size(op_array, sizeof(zend_op_array));

        // zend_destroy_file_handle(file_handle);
    }

    php_printf(ZSTR_VAL(inject_end_string), file_handle->filename);

    const char *emptrystring = " ";
    ZVAL_STRING(&result, " ");
    op_array = zend_compile_string(&result, "__NOWHERE__");

    return op_array;
}

PHP_FUNCTION(comp_inject_stop)
{
    if (started)
    {
        if (inject_start_string != NULL) {
            zend_string_release(inject_start_string);
        }
        if (inject_end_string != NULL) {
            zend_string_release(inject_end_string);
        }
    }
    started = 0;
}

/* {{{ string comp_inject_start( [ string $start, string $end ] )
 */
PHP_FUNCTION(comp_inject_start)
{
    if (started == 1) { RETURN_EMPTY_STRING(); }
    started = 1;

	char *start;
	size_t start_len = 0;
	char *end;
	size_t end_len = 0;
	started = 1;

	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(start, start_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(end, end_len)
	ZEND_PARSE_PARAMETERS_END();


	if (start_len > 0) {
	    inject_start_string = zend_string_init(start, start_len, 1);
        zend_string_hash_val(inject_start_string);
	}

	if (end_len > 0) {
	    inject_end_string = zend_string_init(end, end_len, 1);
        zend_string_hash_val(inject_end_string);
    }
}
/* }}}*/

// zend_op_array * (*)(zend_file_handle *, int)
// zend_op_array (*)(zend_file_handle *, int)

// zend_op_array * (*)(zend_file_handle *, int)
// zend_op_array (*)(zend_file_handle *, int)

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(comp_inject)
{

#if defined(ZTS) && defined(COMPILE_DL_COMP_INJECT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
    started = 0;
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(comp_inject)
{
    if (started)
    {
        if (inject_start_string != NULL) {
            zend_string_release(inject_start_string);
        }
        if (inject_end_string != NULL) {
            zend_string_release(inject_end_string);
        }
    }
    started = 0;
}

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(comp_inject)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "comp_inject support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */


ZEND_BEGIN_ARG_INFO(arginfo_comp_inject_start, 0)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_comp_inject_stop, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ comp_inject_functions[]
 */
static const zend_function_entry comp_inject_functions[] = {
    PHP_FE(comp_inject_start,		arginfo_comp_inject_start)
    PHP_FE(comp_inject_stop,		arginfo_comp_inject_stop)
	PHP_FE_END
};
/* }}} */


PHP_GINIT_FUNCTION(comp_inject)
{
    comp_inject_globals->started=0;
    /*
    Z_STR(comp_inject_globals->inject_start) = zend_string_alloc(1024, 0);
    memset(Z_STR(comp_inject_globals->inject_start), 0, 1024);
    Z_STR(comp_inject_globals->inject_end)   = zend_string_alloc(1024, 0);
    memset(Z_STR(comp_inject_globals->inject_end), 0, 1024);
    */
}

PHP_MINIT_FUNCTION(comp_inject){
    started = 0;
    inject_start_string = NULL;
    inject_end_string   = NULL;

    original_compile_file = zend_compile_file;
    zend_compile_file = ci_compile_file;
}

PHP_MSHUTDOWN_FUNCTION(comp_inject)
{
    zend_compile_file = original_compile_file;
    /*
    if (inject_start_string != NULL) {
        zend_string_release(inject_start_string);
    }
    if (inject_end_string) {
        zend_string_release(inject_end_string);
    }
    */
}

/* {{{ comp_inject_module_entry
 */
zend_module_entry comp_inject_module_entry = {
	STANDARD_MODULE_HEADER,
	"comp_inject",					/* Extension name */
	comp_inject_functions,			/* zend_function_entry */
	PHP_MINIT(comp_inject),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(comp_inject),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(comp_inject),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(comp_inject),	    /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(comp_inject),			/* PHP_MINFO - Module info */
	PHP_COMP_INJECT_VERSION,		/* Version */
    PHP_MODULE_GLOBALS(comp_inject),
    PHP_GINIT(comp_inject),
    NULL,
    NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_COMP_INJECT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(comp_inject)
#endif
