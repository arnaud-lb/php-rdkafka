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
#include "ext/spl/spl_exceptions.h"
#include "topic.h"
#include "queue.h"
#include "message.h"

static zend_object_handlers object_handlers;
zend_class_entry * ce_kafka_consumer_topic;
zend_class_entry * ce_kafka_kafka_consumer_topic;
zend_class_entry * ce_kafka_producer_topic;
zend_class_entry * ce_kafka_topic;

typedef struct _php_callback {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
} php_callback;

static void kafka_topic_free(zend_object *object) /* {{{ */
{
    kafka_topic_object *intern = php_kafka_from_obj(kafka_topic_object, object);

    if (Z_TYPE(intern->zrk) != IS_UNDEF && intern->rkt) {
        kafka_object *kafka_intern = get_kafka_object(&intern->zrk);
        if (kafka_intern) {
            zend_hash_index_del(&kafka_intern->topics, (zend_ulong)intern);
        }
    }

    zend_object_std_dtor(&intern->std);
}
/* }}} */

static zend_object *kafka_topic_new(zend_class_entry *class_type) /* {{{ */
{
    zend_object* retval;
    kafka_topic_object *intern;

    intern = zend_object_alloc(sizeof(*intern), class_type);
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval = &intern->std;
    retval->handlers = &object_handlers;

    return retval;
}
/* }}} */


static void consume_callback(rd_kafka_message_t *msg, void *opaque)
{
    php_callback *cb = (php_callback*) opaque;
    zval args[1];

    if (!opaque) {
        return;
    }

    if (!cb) {
        return;
    }

    ZVAL_NULL(&args[0]);

    kafka_message_new(&args[0], msg, NULL);

    rdkafka_call_function(&cb->fci, &cb->fcc, NULL, 1, args);

    zval_ptr_dtor(&args[0]);
}

kafka_topic_object * get_kafka_topic_object(zval *zrkt)
{
    kafka_topic_object *orkt = Z_RDKAFKA_P(kafka_topic_object, zrkt);

    if (!orkt->rkt) {
        zend_throw_exception_ex(NULL, 0, "RdKafka\\Topic::__construct() has not been called");
        return NULL;
    }

    return orkt;
}

/* {{{ proto RdKafka\ConsumerTopic::consumeCallback([int $partition, int timeout_ms, mixed $callback]) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume_callback, 0, 0, 3)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, timeout_ms)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consumeCallback)
{
    php_callback cb;
    zend_long partition;
    zend_long timeout_ms;
    long result;
    kafka_topic_object *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "llf", &partition, &timeout_ms, &cb.fci, &cb.fcc) == FAILURE) {
        return;
    }

    if (partition < 0 || partition > 0x7FFFFFFF) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    Z_ADDREF_P(&cb.fci.function_name);

    result = rd_kafka_consume_callback(intern->rkt, partition, timeout_ms, consume_callback, &cb);

    zval_ptr_dtor(&cb.fci.function_name);

    RETURN_LONG(result);
}
/* }}} */

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
    zend_long partition;
    zend_long offset;
    int ret;
    rd_kafka_resp_err_t err;
    kafka_object *kafka_intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "llO", &partition, &offset, &zrkqu, ce_kafka_queue) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    queue_intern = get_kafka_queue_object(zrkqu);
    if (!queue_intern) {
        return;
    }

    kafka_intern = get_kafka_object(&intern->zrk);
    if (!kafka_intern) {
        return;
    }

    if (is_consuming_toppar(kafka_intern, intern->rkt, partition)) {
        zend_throw_exception_ex(
            ce_kafka_exception,
            0,
            "%s:" ZEND_LONG_FMT " is already being consumed by the same Consumer instance",
            rd_kafka_topic_name(intern->rkt),
            partition
        );
        return;
    }

    ret = rd_kafka_consume_start_queue(intern->rkt, partition, offset, queue_intern->rkqu);

    if (ret == -1) {
        err = rd_kafka_last_error();
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    add_consuming_toppar(kafka_intern, intern->rkt, partition);
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
    zend_long partition;
    zend_long offset;
    int ret;
    rd_kafka_resp_err_t err;
    kafka_object *kafka_intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &partition, &offset) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    kafka_intern = get_kafka_object(&intern->zrk);
    if (!kafka_intern) {
        return;
    }

    if (is_consuming_toppar(kafka_intern, intern->rkt, partition)) {
        zend_throw_exception_ex(
            ce_kafka_exception,
            0,
            "%s:" ZEND_LONG_FMT " is already being consumed by the same Consumer instance",
            rd_kafka_topic_name(intern->rkt),
            partition
        );
        return;
    }

    ret = rd_kafka_consume_start(intern->rkt, partition, offset);

    if (ret == -1) {
        err = rd_kafka_last_error();
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    add_consuming_toppar(kafka_intern, intern->rkt, partition);
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
    zend_long partition;
    int ret;
    rd_kafka_resp_err_t err;
    kafka_object *kafka_intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &partition) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    kafka_intern = get_kafka_object(&intern->zrk);
    if (!kafka_intern) {
        return;
    }

    ret = rd_kafka_consume_stop(intern->rkt, partition);

    if (ret == -1) {
        err = rd_kafka_last_error();
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    del_consuming_toppar(kafka_intern, intern->rkt, partition);
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
    zend_long partition;
    zend_long timeout_ms;
    rd_kafka_message_t *message;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &partition, &timeout_ms) == FAILURE) {
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    message = rd_kafka_consume(intern->rkt, partition, timeout_ms);

    if (!message) {
        err = rd_kafka_last_error();
        if (err == RD_KAFKA_RESP_ERR__TIMED_OUT) {
            return;
        }
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    kafka_message_new(return_value, message, NULL);

    rd_kafka_message_destroy(message);
}
/* }}} */

/* {{{ proto RdKafka\Message RdKafka\ConsumerTopic::consumeBatch(int $partition, int $timeout_ms, int $batch_size)
   Consume a batch of messages from a partition */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_consume_batch, 0, 0, 3)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, timeout_ms)
    ZEND_ARG_INFO(0, batch_size)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, consumeBatch)
{
    kafka_topic_object *intern;
    zend_long partition, timeout_ms, batch_size;
    long result, i;
    rd_kafka_message_t **rkmessages;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll", &partition, &timeout_ms, &batch_size) == FAILURE) {
        return;
    }

    if (0 >= batch_size) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for batch_size", batch_size);
        return;
    }

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    rkmessages = malloc(sizeof(*rkmessages) * batch_size);

    result = rd_kafka_consume_batch(intern->rkt, partition, timeout_ms, rkmessages, batch_size);

    if (result == -1) {
        free(rkmessages);
        err = rd_kafka_last_error();
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }

    if (result >= 0) {
        kafka_message_list_to_array(return_value, rkmessages, result);
        for (i = 0; i < result; ++i) {
            rd_kafka_message_destroy(rkmessages[i]);
        }
    }

    free(rkmessages);
}
/* }}} */

/* {{{ proto void RdKafka\ConsumerTopic::offsetStore(int partition, int offset) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_offset_store, 0, 0, 2)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ConsumerTopic, offsetStore)
{
    kafka_topic_object *intern;
    zend_long partition;
    zend_long offset;
    rd_kafka_resp_err_t err;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &partition, &offset) == FAILURE) {
        return;
    }

    if (partition < 0 || partition > 0x7FFFFFFF) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    err = rd_kafka_offset_store(intern->rkt, partition, offset);

    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka___private_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry kafka_consumer_topic_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__ConsumerTopic, consumeQueueStart, arginfo_kafka_consume_queue_start, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeCallback, arginfo_kafka_consume_callback, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeStart, arginfo_kafka_consume_start, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeStop, arginfo_kafka_consume_stop, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consume, arginfo_kafka_consume, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, consumeBatch, arginfo_kafka_consume_batch, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__ConsumerTopic, offsetStore, arginfo_kafka_offset_store, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry kafka_kafka_consumer_topic_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__ConsumerTopic, offsetStore, arginfo_kafka_offset_store, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ proto void RdKafka\ProducerTopic::produce(int $partition, int $msgflags[, string $payload[, string $key[, string $msg_opaque]]])
   Produce and send a single message to broker. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_produce, 0, 0, 2)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, msgflags)
    ZEND_ARG_INFO(0, payload)
    ZEND_ARG_INFO(0, key)
#ifdef HAS_RD_KAFKA_PURGE
    ZEND_ARG_INFO(0, msg_opaque)
#endif
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ProducerTopic, produce)
{
    zend_long partition;
    zend_long msgflags;
    char *payload = NULL;
    size_t payload_len = 0;
    char *key = NULL;
    size_t key_len = 0;
    zend_string *opaque = NULL;
    int ret;
    rd_kafka_resp_err_t err;
    kafka_topic_object *intern;

#ifdef HAS_RD_KAFKA_PURGE
    ZEND_PARSE_PARAMETERS_START(2, 5)
#else
    ZEND_PARSE_PARAMETERS_START(2, 4)
#endif
        Z_PARAM_LONG(partition)
        Z_PARAM_LONG(msgflags)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING_OR_NULL(payload, payload_len)
        Z_PARAM_STRING_OR_NULL(key, key_len)
#ifdef HAS_RD_KAFKA_PURGE
        Z_PARAM_STR_OR_NULL(opaque)
#endif
    ZEND_PARSE_PARAMETERS_END();

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    if (msgflags != 0 && msgflags != RD_KAFKA_MSG_F_BLOCK) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value '%ld' for $msgflags", msgflags);
        return;
    }

    intern = get_kafka_topic_object(getThis());

    if (opaque != NULL) {
        zend_string_addref(opaque);
    }

    ret = rd_kafka_produce(intern->rkt, partition, msgflags | RD_KAFKA_MSG_F_COPY, payload, payload_len, key, key_len, opaque);

    if (ret == -1) {
        if (opaque != NULL) {
            zend_string_release(opaque);
        }
        err = rd_kafka_last_error();
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }
}
/* }}} */

#ifdef HAVE_RD_KAFKA_MESSAGE_HEADERS
/* {{{ proto void RdKafka\ProducerTopic::producev(int $partition, int $msgflags[, string $payload[, string $key[, array $headers[, int $timestamp_ms[, string msg_opaque]]]]])
   Produce and send a single message to broker (with headers possibility and timestamp). */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_producev, 0, 0, 2)
    ZEND_ARG_INFO(0, partition)
    ZEND_ARG_INFO(0, msgflags)
    ZEND_ARG_INFO(0, payload)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, headers)
    ZEND_ARG_INFO(0, timestamp_ms)
    ZEND_ARG_INFO(0, msg_opaque)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__ProducerTopic, producev)
{
    zend_long partition;
    zend_long msgflags;
    char *payload = NULL;
    size_t payload_len = 0;
    char *key = NULL;
    size_t key_len = 0;
    rd_kafka_resp_err_t err;
    kafka_topic_object *intern;
    kafka_object *kafka_intern;
    HashTable *headersParam = NULL;
    HashPosition headersParamPos;
    char *header_key;
    zval *header_value;
    rd_kafka_headers_t *headers;
    zend_long timestamp_ms = 0;
    zend_bool timestamp_ms_is_null = 0;
    zend_string *opaque = NULL;

#ifdef HAS_RD_KAFKA_PURGE
    ZEND_PARSE_PARAMETERS_START(2, 7)
#else
    ZEND_PARSE_PARAMETERS_START(2, 6)
#endif
        Z_PARAM_LONG(partition)
        Z_PARAM_LONG(msgflags)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING_OR_NULL(payload, payload_len)
        Z_PARAM_STRING_OR_NULL(key, key_len)
        Z_PARAM_ARRAY_HT_OR_NULL(headersParam)
        Z_PARAM_LONG_OR_NULL(timestamp_ms, timestamp_ms_is_null)
#ifdef HAS_RD_KAFKA_PURGE
        Z_PARAM_STR_OR_NULL(opaque)
#endif
    ZEND_PARSE_PARAMETERS_END();

    if (partition != RD_KAFKA_PARTITION_UA && (partition < 0 || partition > 0x7FFFFFFF)) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Out of range value '%ld' for $partition", partition);
        return;
    }

    if (msgflags != 0 && msgflags != RD_KAFKA_MSG_F_BLOCK) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value '%ld' for $msgflags", msgflags);
        return;
    }

    if (timestamp_ms_is_null == 1) {
        timestamp_ms = 0;
    }

    intern = get_kafka_topic_object(getThis());

    if (opaque != NULL) {
        zend_string_addref(opaque);
    }

    if (headersParam != NULL && zend_hash_num_elements(headersParam) > 0) {
        headers = rd_kafka_headers_new(zend_hash_num_elements(headersParam));
        for (zend_hash_internal_pointer_reset_ex(headersParam, &headersParamPos);
                (header_value = zend_hash_get_current_data_ex(headersParam, &headersParamPos)) != NULL &&
                (header_key = rdkafka_hash_get_current_key_ex(headersParam, &headersParamPos)) != NULL;
                zend_hash_move_forward_ex(headersParam, &headersParamPos)) {
            convert_to_string_ex(header_value);
            rd_kafka_header_add(
                headers,
                header_key,
                -1, // Auto detect header title length
                Z_STRVAL_P(header_value),
                Z_STRLEN_P(header_value)
            );
        }
    } else {
        headers = rd_kafka_headers_new(0);
    }

    kafka_intern = get_kafka_object(&intern->zrk);
    if (!kafka_intern) {
        return;
    }

    err = rd_kafka_producev(
            kafka_intern->rk,
            RD_KAFKA_V_RKT(intern->rkt),
            RD_KAFKA_V_PARTITION(partition),
            RD_KAFKA_V_MSGFLAGS(msgflags | RD_KAFKA_MSG_F_COPY),
            RD_KAFKA_V_VALUE(payload, payload_len),
            RD_KAFKA_V_KEY(key, key_len),
            RD_KAFKA_V_TIMESTAMP(timestamp_ms),
            RD_KAFKA_V_HEADERS(headers),
            RD_KAFKA_V_OPAQUE(opaque),
            RD_KAFKA_V_END
    );

    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        rd_kafka_headers_destroy(headers);
        if (opaque != NULL) {
            zend_string_release(opaque);
        }
        zend_throw_exception(ce_kafka_exception, rd_kafka_err2str(err), err);
        return;
    }
}
/* }}} */
#endif

static const zend_function_entry kafka_producer_topic_fe[] = {
    PHP_ME(RdKafka, __construct, arginfo_kafka___private_construct, ZEND_ACC_PRIVATE)
    PHP_ME(RdKafka__ProducerTopic, produce, arginfo_kafka_produce, ZEND_ACC_PUBLIC)
#ifdef HAVE_RD_KAFKA_MESSAGE_HEADERS
    PHP_ME(RdKafka__ProducerTopic, producev, arginfo_kafka_producev, ZEND_ACC_PUBLIC)
#endif
    PHP_FE_END
};

/* {{{ proto string RdKafka\Topic::getName() */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_topic_get_name, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Topic, getName)
{
    kafka_topic_object *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_kafka_topic_object(getThis());
    if (!intern) {
        return;
    }

    RETURN_STRING(rd_kafka_topic_name(intern->rkt));
}
/* }}} */

static const zend_function_entry kafka_topic_fe[] = {
    PHP_ME(RdKafka__Topic, getName, arginfo_kafka_topic_get_name, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_topic_minit(INIT_FUNC_ARGS) { /* {{{ */

    zend_class_entry ce;

    memcpy(&object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    object_handlers.clone_obj = NULL;
    object_handlers.free_obj = kafka_topic_free;
    object_handlers.offset = XtOffsetOf(kafka_topic_object, std);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Topic", kafka_topic_fe);
    ce_kafka_topic = zend_register_internal_class(&ce);
    ce_kafka_topic->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    ce_kafka_topic->create_object = kafka_topic_new;

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "ConsumerTopic", kafka_consumer_topic_fe);
    ce_kafka_consumer_topic = zend_register_internal_class_ex(&ce, ce_kafka_topic);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "KafkaConsumerTopic", kafka_kafka_consumer_topic_fe);
    ce_kafka_kafka_consumer_topic = zend_register_internal_class_ex(&ce, ce_kafka_topic);

    INIT_NS_CLASS_ENTRY(ce, "RdKafka", "ProducerTopic", kafka_producer_topic_fe);
    ce_kafka_producer_topic = zend_register_internal_class_ex(&ce, ce_kafka_topic);
} /* }}} */
