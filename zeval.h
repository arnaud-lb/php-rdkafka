#ifndef incl_ZEVAL_H
#define incl_ZEVAL_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

/***************************************************************************/
#ifdef ZEND_ENGINE_3

typedef zval zeval;

#define ZEVAL(v)      (v)
#define P_ZEVAL(v)  (&(v))

#define MAKE_STD_ZEVAL(v) ZVAL_NULL(&v)
#define ZEVAL_UNINIT(v) ZVAL_NULL(v)

#define ZE_ISDEF(v) (Z_TYPE(v) != IS_UNDEF)
#define ZE_TYPE(v)   Z_TYPE(v)
#define ZE_BVAL(v)   Z_BVAL(v)
#define ZE_LVAL(V)   Z_LVAL(v)
#define ZE_DVAL(v)   Z_DVAL(v)
#define ZE_STRVAL(v) Z_STRVAL(v)
#define ZE_STRLEN(v) Z_STRLEN(v)
#define ZE_ARRVAL(v) Z_ARRVAL(v)
#define ZE_OBJCE(v)  Z_OBJCE(v)

#define ZEVAL_DUP_C
#define ZEVAL_DUP_CC

#define ZEVAL_DEREF(v) ZVAL_DEREF(v)

/***************************************************************************/
#else
# error "Unknown Zend Engine version"
#endif

#endif
