dnl $Id$
dnl config.m4 for extension rdkafka

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(rdkafka, for rdkafka support,
dnl Make sure that the comment is aligned:
dnl [  --with-rdkafka             Include rdkafka support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(rdkafka, whether to enable rdkafka support,
dnl Make sure that the comment is aligned:
dnl [  --enable-rdkafka           Enable rdkafka support])

if test "$PHP_RDKAFKA" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-rdkafka -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/rdkafka.h"  # you most likely want to change this
  dnl if test -r $PHP_RDKAFKA/$SEARCH_FOR; then # path given as parameter
  dnl   RDKAFKA_DIR=$PHP_RDKAFKA
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for rdkafka files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       RDKAFKA_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$RDKAFKA_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the rdkafka distribution])
  dnl fi

  dnl # --with-rdkafka -> add include path
  dnl PHP_ADD_INCLUDE($RDKAFKA_DIR/include)

  dnl # --with-rdkafka -> check for lib and symbol presence
  dnl LIBNAME=rdkafka # you may want to change this
  dnl LIBSYMBOL=rdkafka # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $RDKAFKA_DIR/$PHP_LIBDIR, RDKAFKA_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_RDKAFKALIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong rdkafka lib version or lib not found])
  dnl ],[
  dnl   -L$RDKAFKA_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(RDKAFKA_SHARED_LIBADD)

  PHP_NEW_EXTENSION(rdkafka, rdkafka.c, $ext_shared)
fi
