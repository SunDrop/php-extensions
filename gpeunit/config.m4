dnl $Id$
dnl config.m4 for extension gpeunit

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(gpeunit, for gpeunit support,
dnl Make sure that the comment is aligned:
dnl [  --with-gpeunit             Include gpeunit support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(gpeunit,
  [whether to enable gpeunit support],
  [  --enable-gpeunit           Enable gpeunit support])

if test "$PHP_GPEUNIT" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-gpeunit -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/gpeunit.h"  # you most likely want to change this
  dnl if test -r $PHP_GPEUNIT/$SEARCH_FOR; then # path given as parameter
  dnl   GPEUNIT_DIR=$PHP_GPEUNIT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for gpeunit files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       GPEUNIT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$GPEUNIT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the gpeunit distribution])
  dnl fi

  dnl # --with-gpeunit -> add include path
  dnl PHP_ADD_INCLUDE($GPEUNIT_DIR/include)

  dnl # --with-gpeunit -> check for lib and symbol presence
  dnl LIBNAME=gpeunit # you may want to change this
  dnl LIBSYMBOL=gpeunit # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $GPEUNIT_DIR/$PHP_LIBDIR, GPEUNIT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_GPEUNITLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong gpeunit lib version or lib not found])
  dnl ],[
  dnl   -L$GPEUNIT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(GPEUNIT_SHARED_LIBADD)

  PHP_REQUIRE_CXX()
  PHP_SUBST(GPEUNIT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, GPEUNIT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(gpelib, 1, GPEUNIT_SHARED_LIBADD)

  PHP_NEW_EXTENSION(gpeunit, gpeunit.c, $ext_shared)
fi
