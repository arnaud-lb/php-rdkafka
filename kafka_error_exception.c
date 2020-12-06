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

zend_class_entry * ce_kafka_error;

void create_kafka_error(zval *return_value, const rd_kafka_error_t *error) /* {{{ */
{
    object_init_ex(return_value, ce_kafka_error);

    zend_update_property_string(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("message"), rd_kafka_error_name(error));
    zend_update_property_long(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("code"), rd_kafka_error_code(error));
    zend_update_property_string(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("error_string"), rd_kafka_error_string(error));
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("isFatal"), rd_kafka_error_is_fatal(error));
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("isRetriable"), rd_kafka_error_is_retriable(error));
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(return_value), ZEND_STRL("transactionRequiresAbort"), rd_kafka_error_txn_requires_abort(error));

    Z_ADDREF_P(return_value);
}
/* }}} */

/* {{{ proto RdKafka\KafkaErrorException::__construct(string $message, int $code[, string $error_string, bool $isFatal, bool $isRetriable, bool $transactionRequiresAbort]) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error___construct, 0, 0, 6)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, error_string)
    ZEND_ARG_INFO(0, isFatal)
    ZEND_ARG_INFO(0, isRetriable)
    ZEND_ARG_INFO(0, transactionRequiresAbort)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, __construct)
{
    char *message, *error_string = "";
    size_t message_length = 0, error_string_length = 0;
    zend_bool isFatal = 0, isRetriable = 0, transactionRequiresAbort = 0;
    zend_long code = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|sbbb", &message, &message_length, &code, &error_string, &error_string_length, &isFatal, &isRetriable, &transactionRequiresAbort) == FAILURE) {
        return;
    }

    zend_update_property_string(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("message"), message);
    zend_update_property_long(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("code"), code);
    zend_update_property_string(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("error_string"), error_string);
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("isFatal"), isFatal);
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("isRetriable"), isRetriable);
    zend_update_property_bool(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("transactionRequiresAbort"), transactionRequiresAbort);
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::getErrorString()
    Get name of error */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_get_error_string, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, getErrorString)
{
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("error_string"), 0);

    if (!res || Z_TYPE_P(res) != IS_STRING) {
        return;
    }

    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
}
/* }}} */


/* {{{ proto void RdKafka\KafkaErrorException::isFatal()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_is_fatal, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, isFatal)
{
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("isFatal"), 0);

    if (!res || (Z_TYPE_P(res) != IS_TRUE && Z_TYPE_P(res) != IS_FALSE)) {
        return;
    }

    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::isRetriable()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_is_retriable, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, isRetriable)
{
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("isRetriable"), 0);

    if (!res || (Z_TYPE_P(res) != IS_TRUE && Z_TYPE_P(res) != IS_FALSE)) {
        return;
    }

    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
}
/* }}} */

/* {{{ proto void RdKafka\KafkaErrorException::transactionRequiresAbort()
    Return true if error is fatal */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_error_transaction_requires_abort, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__KafkaErrorException, transactionRequiresAbort)
{
    zval *res;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        return;
    }

    res = rdkafka_read_property(ce_kafka_error, Z_RDKAFKA_PROP_OBJ(getThis()), ZEND_STRL("transactionRequiresAbort"), 0);

    if (!res || (Z_TYPE_P(res) != IS_TRUE && Z_TYPE_P(res) != IS_FALSE)) {
        return;
    }

    ZVAL_DEREF(res);
    ZVAL_COPY(return_value, res);
}
/* }}} */

static const zend_function_entry kafka_error_fe[] = { /* {{{ */
    PHP_ME(RdKafka__KafkaErrorException, __construct, arginfo_kafka_error___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, getErrorString, arginfo_kafka_error_get_error_string, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, isFatal, arginfo_kafka_error_is_fatal, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, isRetriable, arginfo_kafka_error_is_retriable, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__KafkaErrorException, transactionRequiresAbort, arginfo_kafka_error_transaction_requires_abort, ZEND_ACC_PUBLIC)
    PHP_FE_END
}; /* }}} */

void kafka_error_minit() /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "KafkaErrorException", kafka_error_fe);
    ce_kafka_error = zend_register_internal_class_ex(&ce, ce_kafka_exception);

    zend_declare_property_null(ce_kafka_error, ZEND_STRL("error_string"), ZEND_ACC_PRIVATE);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("isFatal"), 0, ZEND_ACC_PRIVATE);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("isRetriable"), 0, ZEND_ACC_PRIVATE);
    zend_declare_property_bool(ce_kafka_error, ZEND_STRL("transactionRequiresAbort"), 0, ZEND_ACC_PRIVATE);
} /* }}} */
