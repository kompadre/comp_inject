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

zend_string *before_code = NULL;
zend_string *after_code  = NULL;
zend_string *filename    = NULL;
unsigned char started = 0;
zend_op_array *op_before, *op_inside, *op_after = NULL;

static zend_op_array *ci_compile_file(zend_file_handle *file_handle, int type) /* {{{ */
{
    if (started == 0) { return compile_file(file_handle, type); }

    char *buf;
    size_t size;
    zend_stream_fixup(file_handle, &buf, &size);

    size_t len = 0;
    if (before_code != NULL) len += ZSTR_LEN(before_code);
    if (size>0) len += size; // + ZSTR_LEN(after_code);
    if (after_code != NULL) len += ZSTR_LEN(after_code);

    zend_string *res = zend_string_alloc(len, 0);
    int offset = 0;
    if (before_code != NULL) {
        memcpy(ZSTR_VAL(res), before_code, ZSTR_LEN(before_code));
        offset += ZSTR_LEN(before_code);
    }

    memcpy(ZSTR_VAL(res) + offset, buf, size);

    offset += size;
    if (after_code != NULL) {
        memcpy(ZSTR_VAL(res) + offset, after_code, ZSTR_LEN(after_code));
        offset += ZSTR_LEN(after_code);
    }
    ZSTR_VAL(res)[offset] = '\0';
    //zend_printf("Bla: %s\n", ZSTR_VAL(res));
    buf = safe_erealloc(buf, len, 1, ZEND_MMAP_AHEAD);
    memcpy(buf, ZSTR_VAL(res), len);
    buf[len] = '\0';
    file_handle->buf = buf;
    file_handle->len = len;
    /*
    php_printf("\n===============\n");
    php_printf("=== %s ===", file_handle->filename);
    php_printf("\n===============\n");
    php_printf("%s\n", buf);
    php_printf("\n===============\n");
zend_string_hash_val(*result);
    return compile_file(file_handle, type);
}

PHP_FUNCTION(comp_inject_stop)
{
    started = 0;
}

PHP_MINIT_FUNCTION(comp_inject)
{
    original_compile_file = zend_compile_file;
    zend_compile_file = ci_compile_file;
}

PHP_MSHUTDOWN_FUNCTION(comp_inject)
{
    zend_compile_file = original_compile_file;
    if (ZSTR_LEN(before_code) > 0) {
        zend_string_release(before_code);
    }
    if (ZSTR_LEN(after_code) > 0) {
        zend_string_release(after_code);
    }
}

PHP_RSHUTDOWN_FUNCTION(comp_inject)
{
    started = 0;
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
	    before_code = zend_string_init(start, start_len, 1);
	    zend_string_hash_val(*result);
	}
	if (end_len > 0)
	{
	    after_code = zend_string_init(end, end_len, 1);
	}
	started = 1;
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

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(comp_inject)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "comp_inject support", "enabled");
    php_info_print_table_row(2, "comp_inject version", PHP_COMP_INJECT_VERSION);
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

/* {{{ comp_inject_module_entry
 */
zend_module_entry comp_inject_module_entry = {
	STANDARD_MODULE_HEADER,
	"comp_inject",					/* Extension name */
	comp_inject_functions,			/* zend_function_entry */
	PHP_MINIT(comp_inject),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(comp_inject),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(comp_inject),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(comp_inject),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(comp_inject),			/* PHP_MINFO - Module info */
	PHP_COMP_INJECT_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COMP_INJECT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(comp_inject)
#endif
