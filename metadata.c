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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_rdkafka.h"
#include "librdkafka/rdkafka.h"
#include "metadata_collection.h"
#include "metadata_topic.h"
#include "metadata_broker.h"
#include "metadata_partition.h"
#include "Zend/zend_exceptions.h"

typedef struct _object_intern {
    zend_object               std;
    const rd_kafka_metadata_t *metadata;
} object_intern;

static HashTable *get_debug_info(zval *object, int *is_temp TSRMLS_DC);

static zend_class_entry * ce;
static zend_object_handlers handlers;

static void brokers_collection(zval *return_value, zval *parent, object_intern *intern TSRMLS_DC) { /* {{{ */
    kafka_metadata_collection_init(return_value, parent, intern->metadata->brokers, intern->metadata->broker_cnt, sizeof(*intern->metadata->brokers), kafka_metadata_broker_ctor TSRMLS_CC);
}
/* }}} */

static void topics_collection(zval *return_value, zval *parent, object_intern *intern TSRMLS_DC) { /* {{{ */
    kafka_metadata_collection_init(return_value, parent, intern->metadata->topics, intern->metadata->topic_cnt, sizeof(*intern->metadata->topics), kafka_metadata_topic_ctor TSRMLS_CC);
}
/* }}} */

static void kafka_metadata_free(void *object TSRMLS_DC) /* {{{ */
{
    object_intern *intern = (object_intern*)object;

    if (intern->metadata) {
        rd_kafka_metadata_destroy(intern->metadata);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static zend_object_value kafka_metadata_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    object_intern *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_metadata_free, NULL TSRMLS_CC);
    retval.handlers = &handlers;

    return retval;
}
/* }}} */

static object_intern * get_object(zval *zmetadata TSRMLS_DC)
{
    object_intern *ometadata = (object_intern*)zend_object_store_get_object(zmetadata TSRMLS_CC);

    if (!ometadata->metadata) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Metadata::__construct() has not been called");
        return NULL;
    }

    return ometadata;
}

static HashTable *get_debug_info(zval *object, int *is_temp TSRMLS_DC) /* {{{ */
{
    zval ary;
    object_intern *intern;
    zval *brokers;
    zval *topics;

    *is_temp = 1;

    array_init(&ary);

    intern = get_object(object TSRMLS_CC);
    if (!intern) {
        return Z_ARRVAL(ary);
    }

    ALLOC_INIT_ZVAL(brokers);
    brokers_collection(brokers, object, intern TSRMLS_CC);
    add_assoc_zval(&ary, "brokers", brokers);

    ALLOC_INIT_ZVAL(topics);
    topics_collection(topics, object, intern TSRMLS_CC);
    add_assoc_zval(&ary, "topics", topics);

    add_assoc_long(&ary, "orig_broker_id", intern->metadata->orig_broker_id);
    add_assoc_string(&ary, "orig_broker_name", intern->metadata->orig_broker_name, 1);

    return Z_ARRVAL(ary);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata::getOrigBrokerId()
   Broker originating this metadata */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_orig_broker_id, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata, getOrigBrokerId)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    RETURN_LONG(intern->metadata->orig_broker_id);
}
/* }}} */

/* {{{ proto string RdKafka\Metadata::getOrigBrokerName()
   Name of originating broker */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_orig_broker_name, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata, getOrigBrokerName)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    RETURN_STRING(intern->metadata->orig_broker_name, 1);
}
/* }}} */

/* {{{ proto RdKafka\Metadata\Collection RdKafka\Metadata::getBrokers()
   Topics */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_brokers, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata, getBrokers)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    brokers_collection(return_value, this_ptr, intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto RdKafka\Metadata\Collection RdKafka\Metadata::getTopics()
   Topics */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_topics, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata, getTopics)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    topics_collection(return_value, this_ptr, intern TSRMLS_CC);
}
/* }}} */

static const zend_function_entry kafka_metadata_fe[] = {
    PHP_ME(RdKafka__Metadata, getOrigBrokerId, arginfo_kafka_metadata_get_orig_broker_id, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata, getOrigBrokerName, arginfo_kafka_metadata_get_orig_broker_name, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata, getBrokers, arginfo_kafka_metadata_get_brokers, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata, getTopics, arginfo_kafka_metadata_get_topics, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_metadata_minit(TSRMLS_D)
{
    zend_class_entry tmpce;

    INIT_NS_CLASS_ENTRY(tmpce, "RdKafka", "Metadata", kafka_metadata_fe);
    ce = zend_register_internal_class(&tmpce TSRMLS_CC);
    ce->create_object = kafka_metadata_new;

    memcpy(&handlers, &kafka_object_handlers, sizeof(handlers));
    handlers.get_debug_info = get_debug_info;

    kafka_metadata_topic_minit(TSRMLS_C);
    kafka_metadata_broker_minit(TSRMLS_C);
    kafka_metadata_partition_minit(TSRMLS_C);
    kafka_metadata_collection_minit(TSRMLS_C);
}

void kafka_metadata_init(zval *return_value, const rd_kafka_metadata_t *metadata TSRMLS_DC)
{
    object_intern *intern;

    if (object_init_ex(return_value, ce) != SUCCESS) {
        return;
    }

    intern = (object_intern*)zend_object_store_get_object(return_value TSRMLS_CC);
    if (!intern) {
        return;
    }

    intern->metadata = metadata;
}
