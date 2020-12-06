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

typedef struct _object_intern {
    zval                            zmetadata;
    const rd_kafka_metadata_broker_t *metadata_broker;
    zend_object                     std;
} object_intern;

static HashTable *get_debug_info(Z_RDKAFKA_OBJ *object, int *is_temp);

static zend_class_entry * ce;
static zend_object_handlers handlers;

static void free_object(zend_object *object) /* {{{ */
{
    object_intern *intern = php_kafka_from_obj(object_intern, object);

    if (intern->metadata_broker) {
        zval_dtor(&intern->zmetadata);
    }

    zend_object_std_dtor(&intern->std);
}
/* }}} */

static zend_object *create_object(zend_class_entry *class_type) /* {{{ */
{
    zend_object* retval;
    object_intern *intern;

    intern = ecalloc(1, sizeof(object_intern)+ zend_object_properties_size(class_type));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval = &intern->std;
    retval->handlers = &handlers;

    return retval;
}
/* }}} */

static object_intern * get_object(zval *zmt)
{
    object_intern *omt = Z_RDKAFKA_P(object_intern, zmt);

    if (!omt->metadata_broker) {
        zend_throw_exception_ex(NULL, 0, "RdKafka\\Metadata\\Broker::__construct() has not been called");
        return NULL;
    }

    return omt;
}

static HashTable *get_debug_info(Z_RDKAFKA_OBJ *object, int *is_temp) /* {{{ */
{
    zval ary;
    object_intern *intern;

    *is_temp = 1;

    array_init(&ary);

    intern = rdkafka_get_debug_object(object_intern, object);
    if (!intern) {
        return Z_ARRVAL(ary);
    }

    add_assoc_long(&ary, "id", intern->metadata_broker->id);
    add_assoc_string(&ary, "host", intern->metadata_broker->host);
    add_assoc_long(&ary, "port", intern->metadata_broker->port);

    return Z_ARRVAL(ary);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Broker::getId()
   Broker id */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_id, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Broker, getId)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(getThis());
    if (!intern) {
        return;
    }

    RETURN_LONG(intern->metadata_broker->id);
}
/* }}} */

/* {{{ proto string RdKafka\Metadata\Broker::getHost()
   Broker hostname */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_host, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Broker, getHost)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(getThis());
    if (!intern) {
        return;
    }

    RETURN_STRING(intern->metadata_broker->host);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Broker::getPort()
   Broker port */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_get_port, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Broker, getPort)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(getThis());
    if (!intern) {
        return;
    }

    RETURN_LONG(intern->metadata_broker->port);
}
/* }}} */

static const zend_function_entry fe[] = {
    PHP_ME(RdKafka__Metadata__Broker, getId, arginfo_kafka_metadata_get_id, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Broker, getHost, arginfo_kafka_metadata_get_host, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Broker, getPort, arginfo_kafka_metadata_get_port, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_metadata_broker_minit(INIT_FUNC_ARGS)
{
    zend_class_entry tmpce;

    INIT_NS_CLASS_ENTRY(tmpce, "RdKafka", "Metadata\\Broker", fe);
    ce = zend_register_internal_class(&tmpce);
    ce->create_object = create_object;

    handlers = kafka_default_object_handlers;
    handlers.get_debug_info = get_debug_info;
    handlers.free_obj = free_object;
    handlers.offset = XtOffsetOf(object_intern, std);
}

void kafka_metadata_broker_ctor(zval *return_value, zval *zmetadata, const void *data)
{
    rd_kafka_metadata_broker_t *metadata_broker = (rd_kafka_metadata_broker_t*)data;
    object_intern *intern;

    if (object_init_ex(return_value, ce) != SUCCESS) {
        return;
    }

    intern = Z_RDKAFKA_P(object_intern, return_value);
    if (!intern) {
        return;
    }

    ZVAL_ZVAL(&intern->zmetadata, zmetadata, 1, 0);
    intern->metadata_broker = metadata_broker;
}
