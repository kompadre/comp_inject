dnl config.m4 for extension comp_inject

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([comp_inject],
dnl   [for comp_inject support],
dnl   [AS_HELP_STRING([--with-comp_inject],
dnl     [Include comp_inject support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([comp_inject],
  [whether to enable comp_inject support],
  [AS_HELP_STRING([--enable-comp_inject],
    [Enable comp_inject support])],
  [no])

if test "$PHP_COMP_INJECT" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, COMP_INJECT_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-comp_inject -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/comp_inject.h"  # you most likely want to change this
  dnl if test -r $PHP_COMP_INJECT/$SEARCH_FOR; then # path given as parameter
  dnl   COMP_INJECT_DIR=$PHP_COMP_INJECT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for comp_inject files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       COMP_INJECT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$COMP_INJECT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the comp_inject distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-comp_inject -> add include path
  dnl PHP_ADD_INCLUDE($COMP_INJECT_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-comp_inject -> check for lib and symbol presence
  dnl LIBNAME=COMP_INJECT # you may want to change this
  dnl LIBSYMBOL=COMP_INJECT # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_COMP_INJECT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your comp_inject library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $COMP_INJECT_DIR/$PHP_LIBDIR, COMP_INJECT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_COMP_INJECT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your comp_inject library.])
  dnl ],[
  dnl   -L$COMP_INJECT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(COMP_INJECT_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_COMP_INJECT, 1, [ Have comp_inject support ])

  PHP_NEW_EXTENSION(comp_inject, comp_inject.c, $ext_shared)
fi
