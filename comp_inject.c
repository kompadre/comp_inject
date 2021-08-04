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

static zend_op_array *ci_compile_file(zend_file_handle *file_handle, int type);
zend_op_array *(*original_compile_file)(zend_file_handle *file_handle, int type);

ZEND_BEGIN_MODULE_GLOBALS(comp_inject)
    zval inject_start;
    zval inject_end;
ZEND_END_MODULE_GLOBALS(comp_inject)

ZEND_DECLARE_MODULE_GLOBALS(comp_inject)


#ifdef ZTS
#define COMPI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(comp_inject, v)
#else
#define COMPI_G(v) (comp_inject_globals.v)
#endif

static zend_op_array *ci_compile_file(zend_file_handle *file_handle, int type) /* {{{ */
{
    zend_op_array *op_array;
    zval pv;
    zval retval;                                 /* return value */

    
    if (!file_handle) {
        return original_compile_file(file_handle, type);
    }

    zend_first_try {

    if (Z_TYPE_INFO(COMPI_G(inject_start)) == IS_STRING) {
            op_array = zend_compile_string( &COMPI_G(inject_start), (char *)file_handle->filename);
            if (op_array) {
                zend_execute(op_array, &retval);
                destroy_op_array(op_array);
                efree_size(op_array, sizeof(zend_op_array));
            }
    }

    op_array = original_compile_file(file_handle, type);

    if (Z_TYPE_INFO(COMPI_G(inject_end)) == IS_STRING) {
        if (op_array) {
            zend_execute(op_array, &retval);
            destroy_op_array(op_array);
            efree_size(op_array, sizeof(zend_op_array));
        }
       op_array = zend_compile_string( &COMPI_G(inject_end), (char *)file_handle->filename);
    }

    } zend_end_try();

    return op_array;
}

PHP_FUNCTION(comp_inject_stop)
{
    zend_compile_file = original_compile_file;
}

/* {{{ string comp_inject_start( [ string $start, string $end ] )
 */
PHP_FUNCTION(comp_inject_start)
{
	char *start = "";
	size_t start_len = 0;
	char *end = "";
	size_t end_len = 0;

	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(start, start_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(end, end_len)
	ZEND_PARSE_PARAMETERS_END();

	if (start_len > 0) {
	    // Z_STR(COMPI_G(inject_start)) = zend_string_alloc(start_len, 0);
	    memcpy(Z_STRVAL(COMPI_G(inject_start)), start, start_len);
        // Z_STRVAL(COMPI_G(inject_start))[start_len] = '\0';
	    Z_TYPE_INFO(COMPI_G(inject_start)) = IS_STRING;
	}
	if (end_len > 0) {
	    // Z_STR(COMPI_G(inject_end)) = zend_string_alloc(end_len, 0);
	    memcpy(Z_STRVAL(COMPI_G(inject_end)), end, end_len);
        Z_STRVAL(COMPI_G(inject_end))[start_len] = '\0';
	    Z_TYPE_INFO(COMPI_G(inject_end)) = IS_STRING;
	}

    original_compile_file = zend_compile_file;
    zend_compile_file = ci_compile_file;
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

	return SUCCESS;
}
/* }}} */

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
	ZEND_ARG_INFO(0, str)
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
    Z_STR(comp_inject_globals->inject_start) = zend_string_alloc(1024, 0);
    memset(Z_STR(comp_inject_globals->inject_start), 0, 1024);
    Z_STR(comp_inject_globals->inject_end)   = zend_string_alloc(1024, 0);
    memset(Z_STR(comp_inject_globals->inject_end), 0, 1024);
}

/* {{{ comp_inject_module_entry
 */
zend_module_entry comp_inject_module_entry = {
	STANDARD_MODULE_HEADER,
	"comp_inject",					/* Extension name */
	comp_inject_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(comp_inject),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
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
