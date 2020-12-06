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
#include "librdkafka/rdkafka.h"
#include "ext/spl/spl_iterators.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "topic.h"
#include "queue.h"
#include "message.h"

zend_class_entry * ce_kafka_queue;

static zend_object_handlers handlers;

static void kafka_queue_free(zend_object *object) /* {{{ */
{
    kafka_queue_object *intern = php_kafka_from_obj(kafka_queue_object, object);

    if (intern->rkqu) {
        kafka_object *kafka_intern = get_kafka_object(&intern->zrk);
        if (kafka_intern) {
            zend_hash_index_del(&kafka_intern->queues, (zend_ulong)intern);
        }
    }

    zend_object_std_dtor(&intern->std);
}
/* }}} */

static zend_object *kafka_queue_new(zend_class_entry *class_type) /* {{{ */
{
    zend_object* retval;
    kafka_queue_object *intern;

    intern = ecalloc(1, sizeof(kafka_queue_object)+ zend_object_properties_size(class_type));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval = &intern->std;
    retval->handlers = &handlers;

    return retval;
}
/* }}} */

kafka_queue_object * get_kafka_queue_object(zval *zrkqu)
{
    kafka_queue_object *orkqu = Z_RDKAFKA_P(kafka_queue_object, zrkqu);

    if (!orkqu->rkqu) {
        zend_throw_exception_ex(NULL, 0, "RdKafka\\Queue::__construct() has not been called");
        return NULL;
    }

    return orkqu;
}

/* {{{ proto RdKafka\Message RdKafka\Queue::consume(int timeout_ms)
   Consume a single message */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_queue_consume, 0, 0, 1)
    ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Queue, consume)
{
    kafka_queue_object *intern;
    zend_long timeout_ms;
    rd_kafka_message_t *message;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &timeout_ms) == FAILURE) {
        return;
    }

    intern = get_kafka_queue_object(getThis());
    if (!intern) {
        return;
    }

    message = rd_kafka_consume_queue(intern->rkqu, timeout_ms);

    if (!message) {
        err = rd_kafka_last_error();
        if (err == RD_KAFKA_RESP_ERR__TIMED_OUT) {
            return;
        }
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    kafka_message_new(return_value, message);

    rd_kafka_message_destroy(message);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka___private_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry kafka_queue_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__Queue, consume, arginfo_kafka_queue_consume, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_queue_minit(INIT_FUNC_ARGS) { /* {{{ */

    zend_class_entry ce;

    handlers = kafka_default_object_handlers;
    handlers.free_obj = kafka_queue_free;
    handlers.offset = XtOffsetOf(kafka_queue_object, std);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Queue", kafka_queue_fe);
    ce_kafka_queue = zend_register_internal_class(&ce);
    ce_kafka_queue->create_object = kafka_queue_new;
} /* }}} */
