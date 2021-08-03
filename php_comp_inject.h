/* comp_inject extension for PHP (c) 2021 kompadre@gmail.com */

#ifndef PHP_COMP_INJECT_H
# define PHP_COMP_INJECT_H

extern zend_module_entry comp_inject_module_entry;
# define phpext_comp_inject_ptr &comp_inject_module_entry

# define PHP_COMP_INJECT_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_COMP_INJECT)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_COMP_INJECT_H */
