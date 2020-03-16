/*
  +----------------------------------------------------------------------+
  | php-rdkafka                                                          |
  +----------------------------------------------------------------------+
  | Copyright (c) 2016 Arnaud Le Blanc                                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Arnaud Le Blanc <arnaud.lb@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_rdkafka.h"
#include "php_rdkafka_priv.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "kafka_error_exception.h"
#include "zeval.h"

typedef struct _object_intern {
    zend_object std;
} object_intern;

zend_class_entry * ce_kafka_error;
static zend_object_handlers handlers;

static void kafka_error_free(zend_object *object TSRMLS_DC) /* {{{ */
{
    object_intern *intern = get_custom_object(object_intern, object);

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    free_custom_object(intern);
}
/* }}} */

void kafka_error_new(zval *return_value, const rd_kafka_error_t *error TSRMLS_DC) /* {{{ */
{
    zval errorCode, message;

    ZVAL_LONG(&errorCode, rd_kafka_error_code(error));
    RDKAFKA_ZVAL_STRING(&message, rd_kafka_error_name(error));

    object_init_ex(return_value, ce_kafka_error);

    zend_call_method_with_2_params(return_value, ce_kafka_exception, NULL, "__construct", NULL, &message, &errorCode);

    zend_update_property_string(ce_kafka_error, return_value, ZEND_STRL("string"), rd_kafka_error_string(error) TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, return_value, ZEND_STRL("isFatal"), rd_kafka_error_is_fatal(error) TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, return_value, ZEND_STRL("isRetriable"), rd_kafka_error_is_retriable(error) TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, return_value, ZEND_STRL("transactionRequiresAbort"), rd_kafka_error_txn_requires_abort(error) TSRMLS_CC);
}
/* }}} */

object_intern * get_object(zval *zerr TSRMLS_DC) /* {{{ */
{
    object_intern *oerr = get_custom_object_zval(object_intern, zerr);

    return oerr;
}
/* }}} */

/* {{{ proto RdKafka\KafkaErrorException::__construct(string $message, int $code[, string $string, bool $isFatal, bool $isRetriable, bool $transactionRequiresAbort]) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error___construct, 0, 0, 6)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, string)
    ZEND_ARG_INFO(0, isFatal)
    ZEND_ARG_INFO(0, isRetriable)
    ZEND_ARG_INFO(0, transactionRequiresAbort)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, __construct)
{
    char *message, *string = "";
    arglen_t message_length = 0, string_length = 0;
    zend_bool isFatal = 0, isRetriable = 0, transactionRequiresAbort = 0;
    zend_long code = 0;
    zval errorCode, errorMessage;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|sbbb", &message, &message_length, &code, &string, &string_length, &isFatal, &isRetriable, &transactionRequiresAbort) == FAILURE) {
        return;
    }

    ZVAL_LONG(&errorCode, code);
    RDKAFKA_ZVAL_STRING(&errorMessage, message);
    zend_call_method_with_2_params(getThis(), ce_kafka_exception, NULL, "__construct", NULL, &errorMessage, &errorCode);

    zend_update_property_string(ce_kafka_error, getThis(), ZEND_STRL("string"), string TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, getThis(), ZEND_STRL("isFatal"), isFatal TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, getThis(), ZEND_STRL("isRetriable"), isRetriable TSRMLS_CC);
    zend_update_property_bool(ce_kafka_error, getThis(), ZEND_STRL("transactionRequiresAbort"), transactionRequiresAbort TSRMLS_CC);
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::getString()
    Get name of error */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_get_string, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, getString)
{
    object_intern *intern;
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_object(getThis() TSRMLS_CC);

    if (!intern) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, getThis(), ZEND_STRL("string"), 0 TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
#else
    RETURN_ZVAL(res, 1, 0)
#endif
}
/* }}} */


/* {{{ proto void RdKafka\KafkaErrorException::isFatal()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_is_fatal, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, isFatal)
{
    object_intern *intern;
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_object(getThis() TSRMLS_CC);

    if (!intern) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, getThis(), ZEND_STRL("isFatal"), 0 TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
#else
    RETURN_ZVAL(res, 1, 0)
#endif
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::isRetriable()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_is_retriable, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, isRetriable)
{
    object_intern *intern;
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_object(getThis() TSRMLS_CC);

    if (!intern) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, getThis(), ZEND_STRL("isRetriable"), 0 TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
#else
    RETURN_ZVAL(res, 1, 0)
#endif
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::transactionRequiresAbort()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_transaction_requires_abort, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, transactionRequiresAbort)
{
    object_intern *intern;
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_object(getThis() TSRMLS_CC);

    if (!intern) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, getThis(), ZEND_STRL("transactionRequiresAbort"), 0 TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
#else
    RETURN_ZVAL(res, 1, 0)
#endif
}
/* }}} */

static const zend_function_entry kafka_error_fe[] = { /* {{{ */
    PHP_ME(RdKafka__KafkaErrorException, __construct, arginfo_kafka_error___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, getString, arginfo_kafka_error_get_string, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, isFatal, arginfo_kafka_error_is_fatal, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, isRetriable, arginfo_kafka_error_is_retriable, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, transactionRequiresAbort, arginfo_kafka_error_transaction_requires_abort, ZEND_ACC_PUBLIC)
    PHP_FE_END
}; /* }}} */

void kafka_error_minit(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    handlers = kafka_default_object_handlers;
    set_object_handler_free_obj(&handlers, kafka_error_free);
    set_object_handler_offset(&handlers, XtOffsetOf(object_intern, std));

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "KafkaErrorException", kafka_error_fe);
    ce_kafka_error = rdkafka_register_internal_class_ex(&ce, ce_kafka_exception TSRMLS_CC);

    zend_declare_property_null(ce_kafka_error, ZEND_STRL("string"), ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("isFatal"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("isRetriable"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("transactionRequiresAbort"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
} /* }}} */
