/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rdkafka.h"
#include "librdkafka/rdkafka.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

typedef struct _kafka_object {
    zend_object     std;
    rd_kafka_type_t type;
    rd_kafka_t      *rk;
} kafka_object;

typedef enum {
    KAFKA_CONF = 1,
    KAFKA_TOPIC_CONF
} kafka_conf_type;

typedef struct _kafka_conf_object {
    zend_object     std;
    kafka_conf_type type;
    union {
        rd_kafka_conf_t         *conf;
        rd_kafka_topic_conf_t   *topic_conf;
    } u;
} kafka_conf_object;

typedef struct _kafka_queue_object {
    zend_object         std;
    rd_kafka_queue_t    *rkqu;
} kafka_queue_object;

typedef struct _kafka_topic_object {
    zend_object         std;
    rd_kafka_topic_t    *rkt;
    zval                *zrk;
} kafka_topic_object;

static kafka_conf_object * get_kafka_conf_object(zval *zconf);

static const zend_function_entry empty_function_entries[] = {
    PHP_FE_END
};

static zend_object_handlers kafka_object_handlers;
static zend_object_handlers kafka_topic_object_handlers;

static zend_class_entry * ce_kafka;
static zend_class_entry * ce_kafka_conf;
static zend_class_entry * ce_kafka_consumer;
static zend_class_entry * ce_kafka_consumer_topic;
static zend_class_entry * ce_kafka_exception;
static zend_class_entry * ce_kafka_message;
static zend_class_entry * ce_kafka_producer;
static zend_class_entry * ce_kafka_producer_topic;
static zend_class_entry * ce_kafka_queue;
static zend_class_entry * ce_kafka_topic;
static zend_class_entry * ce_kafka_topic_conf;

static void kafka_free(void *object TSRMLS_DC) /* {{{ */
{
    kafka_object *intern = (kafka_object*)object;

    if (intern->rk) {
        while (rd_kafka_outq_len(intern->rk) > 0) {
            rd_kafka_poll(intern->rk, 50);
        }
        rd_kafka_destroy(intern->rk);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static void kafka_init(zval *this_ptr, rd_kafka_type_t type, zval *zconf, zval *zerrstr) /* {{{ */
{
    char *errstr;
    rd_kafka_t *rk;
    kafka_object *intern;
    kafka_conf_object *conf_intern;
    rd_kafka_conf_t *conf = NULL;

    if (zerrstr) {
        zval_dtor(zerrstr);
        ZVAL_NULL(zerrstr);
    }

    if (zconf) {
        conf_intern = get_kafka_conf_object(zconf);
        if (conf_intern) {
            conf = rd_kafka_conf_dup(conf_intern->u.conf);
        }
    }

    errstr = ecalloc(1, 512);

    rk = rd_kafka_new(type, conf, errstr, 512);

    if (errstr[0] && zerrstr) {
        ZVAL_STRING(zerrstr, errstr, 0);
    } else {
        efree(errstr);
    }

    intern = (kafka_object*)zend_object_store_get_object(this_ptr TSRMLS_CC);
    intern->type = type;
    intern->rk = rk;
}
/* }}} */

static zend_object_value kafka_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    kafka_object *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_free, NULL TSRMLS_CC);
    retval.handlers = &kafka_object_handlers;

    return retval;    
}
/* }}} */

static kafka_object * get_kafka_object(zval *zrk)
{
    kafka_object *ork = (kafka_object*)zend_object_store_get_object(zrk);

    if (!ork->rk) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Kafka::__construct() has not been called");
        return NULL;
    }

    return ork;
}

static void kafka_conf_free(void *object TSRMLS_DC) /* {{{ */
{
    kafka_conf_object *intern = (kafka_conf_object*)object;

    switch (intern->type) {
        case KAFKA_CONF:
            if (intern->u.conf) {
                rd_kafka_conf_destroy(intern->u.conf);
            }
            break;
        case KAFKA_TOPIC_CONF:
            if (intern->u.topic_conf) {
                rd_kafka_topic_conf_destroy(intern->u.topic_conf);
            }
            break;
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static zend_object_value kafka_conf_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    kafka_conf_object *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_conf_free, NULL TSRMLS_CC);
    retval.handlers = &kafka_object_handlers;

    return retval;    
}
/* }}} */

static kafka_conf_object * get_kafka_conf_object(zval *zconf)
{
    kafka_conf_object *oconf = (kafka_conf_object*)zend_object_store_get_object(zconf);

    if (!oconf->type) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Conf::__construct() has not been called");
        return NULL;
    }

    return oconf;
}

static void kafka_queue_free(void *object TSRMLS_DC) /* {{{ */
{
    kafka_queue_object *intern = (kafka_queue_object*)object;

    if (intern->rkqu) {
        rd_kafka_queue_destroy(intern->rkqu);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static zend_object_value kafka_queue_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    kafka_queue_object *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_queue_free, NULL TSRMLS_CC);
    retval.handlers = &kafka_object_handlers;

    return retval;
}
/* }}} */

static kafka_queue_object * get_kafka_queue_object(zval *zrkqu)
{
    kafka_queue_object *orkqu = (kafka_queue_object*)zend_object_store_get_object(zrkqu);

    if (!orkqu->rkqu) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Queue::__construct() has not been called");
        return NULL;
    }

    return orkqu;
}

static void kafka_topic_free(void *object TSRMLS_DC) /* {{{ */
{
    kafka_topic_object *intern = (kafka_topic_object*)object;

    if (intern->rkt) {
        rd_kafka_topic_destroy(intern->rkt);
    }
    if (intern->zrk) {
        zval_ptr_dtor(&intern->zrk);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static zend_object_value kafka_topic_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    kafka_topic_object *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_topic_free, NULL TSRMLS_CC);
    retval.handlers = &kafka_topic_object_handlers;

    return retval;
}
/* }}} */

static kafka_topic_object * get_kafka_topic_object(zval *zrkt)
{
    kafka_topic_object *orkt = (kafka_topic_object*)zend_object_store_get_object(zrkt);

    if (!orkt->rkt) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Topic::__construct() has not been called");
        return NULL;
    }

    return orkt;
}

static void new_message(zval *return_value, rd_kafka_message_t *message)
{
    object_init_ex(return_value, ce_kafka_message);

    zend_update_property_long(NULL, return_value, ZEND_STRL("err"), message->err);
    zend_update_property_string(NULL, return_value, ZEND_STRL("topic_name"), rd_kafka_topic_name(message->rkt));
    zend_update_property_long(NULL, return_value, ZEND_STRL("partition"), message->partition);
    if (message->payload) {
        zend_update_property_stringl(NULL, return_value, ZEND_STRL("payload"), message->payload, message->len);
    }
    if (message->key) {
        zend_update_property_stringl(NULL, return_value, ZEND_STRL("key"), message->key, message->key_len);
    }
    zend_update_property_long(NULL, return_value, ZEND_STRL("offset"), message->offset);
}

/* {{{ private constructor */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka___private_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka, __construct)
{
    zend_throw_exception(NULL, "Private constructor", 0);
}
/* }}} */

/* {{{ proto RdKafka\Conf::__construct() */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_conf___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Conf, __construct)
{
    kafka_conf_object *intern;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        zend_restore_error_handling(&error_handling TSRMLS_CC);
        return;
    }

    intern = (kafka_conf_object*)zend_object_store_get_object(this_ptr TSRMLS_CC);
    intern->type = KAFKA_CONF;
    intern->u.conf = rd_kafka_conf_new();

    zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto array RfKafka\Conf::dump()
   Dump the configuration properties and values of `conf` to an array */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_conf_dump, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Conf, dump)
{
    size_t cntp;
    const char **dump;
    kafka_conf_object *intern;
    size_t i;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_kafka_conf_object(this_ptr);
    if (!intern) {
        return;
    }

    switch (intern->type) {
        case KAFKA_CONF:
            dump = rd_kafka_conf_dump(intern->u.conf, &cntp);
            break;
        case KAFKA_TOPIC_CONF:
            dump = rd_kafka_topic_conf_dump(intern->u.topic_conf, &cntp);
            break;
    }

    array_init(return_value);

    for (i = 0; i < cntp; i+=2) {
        const char *key = dump[i];
        const char *value = dump[i+1];
        add_assoc_string(return_value, (char*)key, (char*)value, 1);
    }

    rd_kafka_conf_dump_free(dump, cntp);
}
/* }}} */

/* {{{ proto void RdKafka\Conf::set(RdKafka\Conf $conf, string $name, string $value[, string &$errstr])
   Sets a configuration property. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_conf_set, 0, 0, 3)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, errstr)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Conf, set)
{
    char *name;
    int name_len;
    char *value;
    int value_len;
    zval *zerrstr = NULL;
    kafka_conf_object *intern;
    rd_kafka_conf_res_t ret;
    char *errstr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|Z", &name, &name_len, &value, &value_len, &zerrstr) == FAILURE) {
        return;
    }

    intern = get_kafka_conf_object(this_ptr);
    if (!intern) {
        return;
    }

    if (zerrstr) {
        zval_dtor(zerrstr);
        ZVAL_NULL(zerrstr);
    }

    errstr = ecalloc(1, 512);

    switch (intern->type) {
        case KAFKA_CONF:
            ret = rd_kafka_conf_set(intern->u.conf, name, value, errstr, 512);
            break;
        case KAFKA_TOPIC_CONF:
            ret = rd_kafka_topic_conf_set(intern->u.topic_conf, name, value, errstr, 512);
            break;
    }

    if (errstr[0] && zerrstr) {
        ZVAL_STRING(zerrstr, errstr, 0);
    } else {
        efree(errstr);
    }

    switch (ret) {
        case RD_KAFKA_CONF_UNKNOWN:
            zend_throw_exception(ce_kafka_exception, "Unknown configuration name", RD_KAFKA_CONF_UNKNOWN);
            break;
        case RD_KAFKA_CONF_INVALID:
            zend_throw_exception(ce_kafka_exception, "Unknown configuration value", RD_KAFKA_CONF_INVALID);
            break;
        case RD_KAFKA_CONF_OK:
            break;
    }
}
/* }}} */

static const zend_function_entry kafka_conf_fe[] = {
    PHP_ME(RdKafka__Conf, __construct, arginfo_kafka_conf___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, dump, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, set, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto void RdKafka\ConsumerTopic::consumeQueueStart(int $partition, int $offset, RdKafka\Queue $queue)
 * Same as consumeStart(), but re-routes incoming messages to the provided queue */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume_queue_start, 0, 0, 3)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consumeQueueStart)
{
    zval *zrkqu;
    kafka_topic_object *intern;
    kafka_queue_object *queue_intern;
    long partition;
    long offset;
    int ret;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llO", &partition, &offset, &zrkqu, ce_kafka_queue) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(this_ptr);
    if (!intern) {
        return;
    }

    queue_intern = get_kafka_queue_object(zrkqu);
    if (!queue_intern) {
        return;
    }

    ret = rd_kafka_consume_start_queue(intern->rkt, partition, offset, queue_intern->rkqu);

    if (ret == -1) {
        err = rd_kafka_errno2err(errno);
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
    }
}
/* }}} */

/* {{{ proto void RdKafka\ConsumerTopic::consumeStart(int partition, int offset)
   Start consuming messages */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume_start, 0, 0, 2)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consumeStart)
{
    kafka_topic_object *intern;
    long partition;
    long offset;
    int ret;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &partition, &offset) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(this_ptr);
    if (!intern) {
        return;
    }

    ret = rd_kafka_consume_start(intern->rkt, partition, offset);

    if (ret == -1) {
        err = rd_kafka_errno2err(errno);
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
    }
}
/* }}} */

/* {{{ proto void RdKafka\ConsumerTopic::consumeStop(int partition)
   Stop consuming messages */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume_stop, 0, 0, 1)
    ZEND_ARG_INFO(0, partition)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consumeStop)
{
    kafka_topic_object *intern;
    long partition;
    int ret;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &partition) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(this_ptr);
    if (!intern) {
        return;
    }

    ret = rd_kafka_consume_stop(intern->rkt, partition);

    if (ret == -1) {
        err = rd_kafka_errno2err(errno);
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
    }
}
/* }}} */

/* {{{ proto RdKafka\Message RdKafka\ConsumerTopic::consume(int $partition, int timeout_ms)
   Consume a single message from partition */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume, 0, 0, 2)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consume)
{
    kafka_topic_object *intern;
    long partition;
    long timeout_ms;
    rd_kafka_message_t *message;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &partition, &timeout_ms) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(this_ptr);
    if (!intern) {
        return;
    }

    message = rd_kafka_consume(intern->rkt, partition, timeout_ms);

    if (!message) {
        err = rd_kafka_errno2err(errno);
        if (err == RD_KAFKA_RESP_ERR__TIMED_OUT) {
            return;
        }
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    new_message(return_value, message);

    rd_kafka_message_destroy(message);
}
/* }}} */

static const zend_function_entry kafka_consumer_topic_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__ConsumerTopic, consumeQueueStart, arginfo_kafka_consume_queue_start, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeStart, arginfo_kafka_consume_start, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeStop, arginfo_kafka_consume_stop, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consume, arginfo_kafka_consume, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto RdKafka\Consumer::__construct([RdKafka\Conf $conf[, string &$errstr]]) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consumer___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, conf)
    ZEND_ARG_INFO(1, errstr)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Consumer, __construct)
{
    zval *zconf = NULL;
    zval *zerrstr = NULL;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|Oz", &zconf, ce_kafka_conf, &zerrstr) == FAILURE) {
        zend_restore_error_handling(&error_handling TSRMLS_CC);
        return;
    }

    kafka_init(this_ptr, RD_KAFKA_CONSUMER, zconf, zerrstr);

    zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

static const zend_function_entry kafka_consumer_fe[] = {
    PHP_ME(RdKafka__Consumer, __construct, arginfo_kafka_consumer___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto int RdKafka\Kafka::addBrokers(string $brokerList)
   Returns the number of brokers successfully added */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_add_brokers, 0, 0, 1)
    ZEND_ARG_INFO(0, broker_list)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, addBrokers)
{
    char *broker_list;
    int broker_list_len;
    kafka_object *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &broker_list, &broker_list_len) == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    RETURN_LONG(rd_kafka_brokers_add(intern->rk, broker_list));
}
/* }}} */

/* {{{ proto void RdKafka\Kafka::setLogLevel(int level)
   Specifies the maximum logging level produced by internal kafka logging and debugging */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_set_log_level, 0, 0, 1)
    ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, setLogLevel)
{
    kafka_object *intern;
    long level;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &level) == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    rd_kafka_set_log_level(intern->rk, level);
}
/* }}} */

/* {{{ proto RdKafka\Topic RdKafka\Kafka::newQueue()
   Returns a RdKafka\Queue object */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_new_queue, 0, 0, 0)
    ZEND_ARG_INFO(0, topic_name)
    ZEND_ARG_INFO(0, topic_conf)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, newQueue)
{
    rd_kafka_queue_t *rkqu;
    kafka_object *intern;
    kafka_queue_object *queue_intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    rkqu = rd_kafka_queue_new(intern->rk);

    if (!rkqu) {
        return;
    }

    if (object_init_ex(return_value, ce_kafka_queue) != SUCCESS) {
        return;
    }

    queue_intern = (kafka_queue_object*)zend_object_store_get_object(return_value TSRMLS_CC);
    if (!queue_intern) {
        return;
    }

    queue_intern->rkqu = rkqu;
}
/* }}} */

/* {{{ proto RdKafka\Topic RdKafka\Kafka::newTopic(string $topic)
   Returns an RdKafka\Topic object */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_new_topic, 0, 0, 1)
    ZEND_ARG_INFO(0, topic_name)
    ZEND_ARG_INFO(0, topic_conf)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, newTopic)
{
    char *topic;
    int topic_len;
    rd_kafka_topic_t *rkt;
    kafka_object *intern;
    kafka_topic_object *topic_intern;
    zend_class_entry *topic_type;
    zval *zconf = NULL;
    rd_kafka_topic_conf_t *conf = NULL;
    kafka_conf_object *conf_intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|O", &topic, &topic_len, &zconf, ce_kafka_topic_conf) == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    if (zconf) {
        conf_intern = get_kafka_conf_object(zconf);
        if (conf_intern) {
            conf = rd_kafka_topic_conf_dup(conf_intern->u.topic_conf);
        }
    }

    rkt = rd_kafka_topic_new(intern->rk, topic, conf);

    if (!rkt) {
        return;
    }

    switch (intern->type) {
        case RD_KAFKA_CONSUMER:
            topic_type = ce_kafka_consumer_topic;
            break;
        case RD_KAFKA_PRODUCER:
            topic_type = ce_kafka_producer_topic;
            break;
    }

    if (object_init_ex(return_value, topic_type) != SUCCESS) {
        return;
    }

    topic_intern = (kafka_topic_object*)zend_object_store_get_object(return_value TSRMLS_CC);
    if (!topic_intern) {
        return;
    }

    topic_intern->rkt = rkt;
    topic_intern->zrk = this_ptr;
    Z_ADDREF_P(this_ptr);
}
/* }}} */

/* {{{ proto int RdKafka\Kafka::outqLen()
   Returns the current out queue length */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_outq_len, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, outqLen)
{
    kafka_object *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    RETURN_LONG(rd_kafka_outq_len(intern->rk));
}
/* }}} */

/* {{{ proto int RdKafka\Kafka::poll()
   Polls the provided kafka handle for events */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_poll, 0, 0, 1)
    ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Kafka, poll)
{
    kafka_object *intern;
    long timeout;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timeout) == FAILURE) {
        return;
    }

    intern = get_kafka_object(this_ptr);
    if (!intern) {
        return;
    }

    RETURN_LONG(rd_kafka_poll(intern->rk, timeout));
}
/* }}} */

static const zend_function_entry kafka_fe[] = {
    PHP_ME(RdKafka__Kafka, addBrokers, arginfo_kafka_add_brokers, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Kafka, setLogLevel, arginfo_kafka_set_log_level, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Kafka, newQueue, arginfo_kafka_new_queue, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Kafka, newTopic, arginfo_kafka_new_topic, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Kafka, outqLen, arginfo_kafka_outq_len, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Kafka, poll, arginfo_kafka_poll, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto string RdKafka\Message::errstr()
 *  Returns the error string for an errored KrKafka\Message or NULL if there was no error.
 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_message_errstr, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Message, errstr)
{
    zval *zerr;
    zval *zpayload;
    const char *errstr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    zerr = zend_read_property(NULL, this_ptr, ZEND_STRL("err"), 0 TSRMLS_CC);

    if (!zerr || Z_TYPE_P(zerr) != IS_LONG) {
        return;
    }

    zpayload = zend_read_property(NULL, this_ptr, ZEND_STRL("payload"), 0 TSRMLS_CC);

    if (zpayload && Z_TYPE_P(zpayload) == IS_STRING) {
        RETURN_ZVAL(zpayload, 1, 0);
    }

    errstr = rd_kafka_err2str(Z_LVAL_P(zerr));

    if (errstr) {
        RETURN_STRING(errstr, 1);
    }
}
/* }}} */

static const zend_function_entry kafka_message_fe[] = {
    PHP_ME(RdKafka__Message, errstr, arginfo_kafka_message_errstr, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto void RdKafka\ProducerTopic::produce(int partition, int msgflags, string payload, string key)
   Produce and send a single message to broker. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_produce, 0, 0, 3)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, msgflags)
    ZEND_ARG_INFO(0, payload)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ProducerTopic, produce)
{
    long partition;
    long msgflags;
    char *payload;
    int payload_len;
    char *key = NULL;
    int key_len = 0;
    int ret;
    rd_kafka_resp_err_t err;
    kafka_topic_object *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls|s", &partition, &msgflags, &payload, &payload_len, &key, &key_len) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Out of range value '%ld' for $partition", partition);
        return;
    }

    if (msgflags != 0) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid value '%ld' for $msgflags", msgflags);
        return;
    }

    intern = get_kafka_topic_object(this_ptr);

    ret = rd_kafka_produce(intern->rkt, partition, msgflags | RD_KAFKA_MSG_F_COPY, payload, payload_len, key, key_len, NULL);

    if (ret == -1) {
        err = rd_kafka_errno2err(errno);
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
    }
}
/* }}} */

static const zend_function_entry kafka_producer_topic_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__ProducerTopic, produce, arginfo_kafka_produce, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto RdKafka\Producer::__construct([RdKafka\Conf $conf[, string &$errstr]]) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_producer___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, conf)
    ZEND_ARG_INFO(1, errstr)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Producer, __construct)
{
    zval *zconf = NULL;
    zval *zerrstr = NULL;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|Oz", &zconf, ce_kafka_conf, &zerrstr) == FAILURE) {
        zend_restore_error_handling(&error_handling TSRMLS_CC);
        return;
    }

    kafka_init(this_ptr, RD_KAFKA_PRODUCER, zconf, zerrstr);

    zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

static const zend_function_entry kafka_producer_fe[] = {
    PHP_ME(RdKafka__Producer, __construct, arginfo_kafka_producer___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto int RdKafka\Queue::consume(int timeout_ms)
   Consume a single message */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_queue_consume, 0, 0, 1)
    ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Queue, consume)
{
    kafka_queue_object *intern;
    long timeout_ms;
    rd_kafka_message_t *message;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timeout_ms) == FAILURE) {
        return;
    }

    intern = get_kafka_queue_object(this_ptr);
    if (!intern) {
        return;
    }

    message = rd_kafka_consume_queue(intern->rkqu, timeout_ms);

    if (!message) {
        err = rd_kafka_errno2err(errno);
        if (err == RD_KAFKA_RESP_ERR__TIMED_OUT) {
            return;
        }
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    new_message(return_value, message);

    rd_kafka_message_destroy(message);
}
/* }}} */

static const zend_function_entry kafka_queue_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__Queue, consume, arginfo_kafka_queue_consume, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto RdKafka\TopicConf::__construct() */
PHP_METHOD(RdKafka__TopicConf, __construct)
{
    kafka_conf_object *intern;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        zend_restore_error_handling(&error_handling TSRMLS_CC);
        return;
    }

    intern = (kafka_conf_object*)zend_object_store_get_object(this_ptr TSRMLS_CC);
    intern->type = KAFKA_TOPIC_CONF;
    intern->u.topic_conf = rd_kafka_topic_conf_new();

    zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

static const zend_function_entry kafka_topic_conf_fe[] = {
    PHP_ME(RdKafka__TopicConf, __construct, arginfo_kafka_conf___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, dump, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, set, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto string rd_kafka_err2str(int $err)
 * Returns a human readable representation of a kafka error.
 */
PHP_FUNCTION(rd_kafka_err2str)
{
    long err;
    const char *errstr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &err) == FAILURE) {
        return;
    }

    errstr = rd_kafka_err2str(err);

    if (errstr) {
        RETURN_STRING(errstr, 1);
    }
}
/* }}} */

/* {{{ proto int rd_kafka_errno()
 * Returns `errno` */
PHP_FUNCTION(rd_kafka_errno)
{
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        return;
    }

    RETURN_LONG(errno);
}
/* }}} */

/* {{{ proto int rd_kafka_errno2err(int $errnox)
 * Converts `errno` to a rdkafka error code */
PHP_FUNCTION(rd_kafka_errno2err)
{
    long errnox;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &errnox) == FAILURE) {
        return;
    }

    RETURN_LONG(rd_kafka_errno2err(errnox));
}
/* }}} */

/* {{{ proto int rd_kafka_thread_cnt()
 * Retrieve the current number of threads in use by librdkafka.
 */
PHP_FUNCTION(rd_kafka_thread_cnt)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    RETURN_LONG(rd_kafka_thread_cnt());
}
/* }}} */

/* {{{ proto int rd_kafka_offset_tail(int $cnt)
 * Start consuming `$cnt` messages from topic's current `.._END` offset.
 */
PHP_FUNCTION(rd_kafka_offset_tail)
{
    long cnt;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &cnt) == FAILURE) {
        return;
    }

    RETURN_LONG(RD_KAFKA_OFFSET_TAIL(cnt));
}
/* }}} */

#define COPY_CONSTANT(name) \
    REGISTER_LONG_CONSTANT(#name, name, CONST_CS | CONST_PERSISTENT)

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rdkafka)
{
    COPY_CONSTANT(RD_KAFKA_CONSUMER);
    COPY_CONSTANT(RD_KAFKA_OFFSET_BEGINNING);
    COPY_CONSTANT(RD_KAFKA_OFFSET_END);
    COPY_CONSTANT(RD_KAFKA_OFFSET_STORED);
    COPY_CONSTANT(RD_KAFKA_PARTITION_UA);
    COPY_CONSTANT(RD_KAFKA_PRODUCER);
    COPY_CONSTANT(RD_KAFKA_VERSION);

    COPY_CONSTANT(RD_KAFKA_RESP_ERR__BEGIN);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__BAD_MSG);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__BAD_COMPRESSION);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__DESTROY);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__FAIL);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__TRANSPORT);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__CRIT_SYS_RESOURCE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__RESOLVE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__MSG_TIMED_OUT);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__PARTITION_EOF);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__FS);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__ALL_BROKERS_DOWN);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__INVALID_ARG);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__TIMED_OUT);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__QUEUE_FULL);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__ISR_INSUFF);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR__END);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_UNKNOWN);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_NO_ERROR);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_OFFSET_OUT_OF_RANGE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_INVALID_MSG);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_UNKNOWN_TOPIC_OR_PART);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_INVALID_MSG_SIZE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_LEADER_NOT_AVAILABLE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_NOT_LEADER_FOR_PARTITION);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_REQUEST_TIMED_OUT);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_BROKER_NOT_AVAILABLE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_REPLICA_NOT_AVAILABLE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_MSG_SIZE_TOO_LARGE);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_STALE_CTRL_EPOCH);
    COPY_CONSTANT(RD_KAFKA_RESP_ERR_OFFSET_METADATA_TOO_LARGE);

    COPY_CONSTANT(RD_KAFKA_CONF_UNKNOWN);
    COPY_CONSTANT(RD_KAFKA_CONF_INVALID);
    COPY_CONSTANT(RD_KAFKA_CONF_OK);

    zend_class_entry ce;

    memcpy(&kafka_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    kafka_object_handlers.clone_obj = NULL;

    memcpy(&kafka_topic_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    kafka_topic_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "RdKafka", kafka_fe);
    ce_kafka = zend_register_internal_class(&ce TSRMLS_CC);
    ce_kafka->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    ce_kafka->create_object = kafka_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Consumer", kafka_consumer_fe);
    ce_kafka_consumer = zend_register_internal_class_ex(&ce, ce_kafka, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Producer", kafka_producer_fe);
    ce_kafka_producer = zend_register_internal_class_ex(&ce, ce_kafka, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Conf", kafka_conf_fe);
    ce_kafka_conf = zend_register_internal_class(&ce TSRMLS_CC);
    ce_kafka_conf->create_object = kafka_conf_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "TopicConf", kafka_topic_conf_fe);
    ce_kafka_topic_conf = zend_register_internal_class(&ce TSRMLS_CC);
    ce_kafka_topic_conf->create_object = kafka_conf_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Topic", empty_function_entries);
    ce_kafka_topic = zend_register_internal_class(&ce TSRMLS_CC);
    ce_kafka_topic->ce_flags = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    ce_kafka_topic->create_object = kafka_topic_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "ConsumerTopic", kafka_consumer_topic_fe);
    ce_kafka_consumer_topic = zend_register_internal_class_ex(&ce, ce_kafka_topic, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "ProducerTopic", kafka_producer_topic_fe);
    ce_kafka_producer_topic = zend_register_internal_class_ex(&ce, ce_kafka_topic, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Message", kafka_message_fe);
    ce_kafka_message = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(ce_kafka_message, ZEND_STRL("err"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(ce_kafka_message, ZEND_STRL("topic_name"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(ce_kafka_message, ZEND_STRL("partition"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(ce_kafka_message, ZEND_STRL("payload"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(ce_kafka_message, ZEND_STRL("key"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(ce_kafka_message, ZEND_STRL("offset"), ZEND_ACC_PUBLIC);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Queue", kafka_queue_fe);
    ce_kafka_queue = zend_register_internal_class(&ce TSRMLS_CC);
    ce_kafka_queue->create_object = kafka_queue_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Exception", NULL);
    ce_kafka_exception = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rdkafka)
{
    rd_kafka_wait_destroyed(1000);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rdkafka)
{
    char *rd_kafka_version;

    php_info_print_table_start();
    php_info_print_table_header(2, "rdkafka support", "enabled");

    php_info_print_table_header(2, "version", PHP_RDKAFKA_VERSION);
    php_info_print_table_header(2, "build date", __DATE__ " " __TIME__);

    spprintf(
        &rd_kafka_version,
        0,
        "%u.%u.%u.%u",
        (RD_KAFKA_VERSION & 0xFF000000) >> 24,
        (RD_KAFKA_VERSION & 0x00FF0000) >> 16,
        (RD_KAFKA_VERSION & 0x0000FF00) >> 8,
        (RD_KAFKA_VERSION & 0x000000FF) >> 8
    );

    php_info_print_table_header(2, "librdkafka version", rd_kafka_version);

    efree(rd_kafka_version);

    php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_err2str, 0, 0, 1)
    ZEND_ARG_INFO(0, err)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_errno2err, 0, 0, 1)
    ZEND_ARG_INFO(0, errnox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_errno, 0, 0, 0)
    ZEND_ARG_INFO(0, errnox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_thread_cnt, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_offset_tail, 0, 0, 1)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ rdkafka_functions[]
 */
const zend_function_entry rdkafka_functions[] = {
    PHP_FE(rd_kafka_err2str,    arginfo_kafka_err2str)
    PHP_FE(rd_kafka_errno2err,  arginfo_kafka_errno2err)
    PHP_FE(rd_kafka_errno,      arginfo_kafka_errno)
    PHP_FE(rd_kafka_offset_tail,arginfo_kafka_offset_tail)
    PHP_FE(rd_kafka_thread_cnt, arginfo_kafka_thread_cnt)
    PHP_FE_END    /* Must be the last line in rdkafka_functions[] */
};
/* }}} */

/* {{{ rdkafka_module_entry
 */
zend_module_entry rdkafka_module_entry = {
    STANDARD_MODULE_HEADER,
    "rdkafka",
    rdkafka_functions,
    PHP_MINIT(rdkafka),
    PHP_MSHUTDOWN(rdkafka),
    NULL,
    NULL,
    PHP_MINFO(rdkafka),
    PHP_RDKAFKA_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_RDKAFKA
ZEND_GET_MODULE(rdkafka)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
