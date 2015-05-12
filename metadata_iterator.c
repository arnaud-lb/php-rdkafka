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
#include "ext/spl/spl_iterators.h"
#include "Zend/zend_interfaces.h"
#include "metadata_iterator.h"
#include "Zend/zend_exceptions.h"

typedef struct _object_intern {
    zend_object                     std;
    zval                            zmetadata;
    const void                      *items;
    size_t                          item_cnt;
    size_t                          item_size;
    size_t                          position;
    kafka_metadata_iterator_ctor_t  ctor;
} object_intern;

static zend_class_entry *ce;

static void free_object(void *object TSRMLS_DC) /* {{{ */
{
    object_intern *intern = (object_intern*)object;

    if (intern->items) {
        zval_dtor(&intern->zmetadata);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);

    efree(intern);
}
/* }}} */

static zend_object_value create_object(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
    zend_object_value retval;
    object_intern *intern;

    intern = ecalloc(1, sizeof(*intern));
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, free_object, NULL TSRMLS_CC);
    retval.handlers = &kafka_object_handlers;

    return retval;
}
/* }}} */

static object_intern * get_object(zval *zmti)
{
    object_intern *omti = (object_intern*)zend_object_store_get_object(zmti);

    if (!omti->items) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Metadata\\Iterator::__construct() has not been called");
        return NULL;
    }

    return omti;
}

/* {{{ proto int RdKafka\Metadata\Iterator::count()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_count, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, count)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    RETURN_LONG(intern->item_cnt);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Iterator::rewind()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_rewind, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, rewind)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    intern->position = 0;
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Iterator::current()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_current, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, current)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    if (intern->position >= intern->item_cnt) {
        zend_throw_exception(ce_kafka_exception, "Called current() on invalid iterator", 0);
        return;
    }

    intern->ctor(return_value, &intern->zmetadata, intern->items + intern->position * intern->item_size);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Iterator::key()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_key, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, key)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    if (intern->position >= intern->item_cnt) {
        zend_throw_exception(ce_kafka_exception, "Called key() on invalid iterator", 0);
        return;
    }

    RETURN_LONG(intern->position);
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Iterator::next()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_next, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, next)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    intern->position ++;
}
/* }}} */

/* {{{ proto int RdKafka\Metadata\Iterator::valid()
   */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_metadata_valid, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Metadata__Iterator, valid)
{
    object_intern *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = get_object(this_ptr);
    if (!intern) {
        return;
    }

    RETURN_BOOL(intern->position < intern->item_cnt);
}
/* }}} */

static const zend_function_entry fe[] = {
    PHP_ME(RdKafka__Metadata__Iterator, count, arginfo_kafka_metadata_count, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Iterator, current, arginfo_kafka_metadata_current, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Iterator, key, arginfo_kafka_metadata_key, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Iterator, next, arginfo_kafka_metadata_next, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Iterator, rewind, arginfo_kafka_metadata_rewind, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Metadata__Iterator, valid, arginfo_kafka_metadata_valid, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_metadata_iterator_minit()
{
    zend_class_entry tmpce;

    INIT_NS_CLASS_ENTRY(tmpce, "RdKafka\\Metadata", "Iterator", fe);
    ce = zend_register_internal_class(&tmpce TSRMLS_CC);
    ce->create_object = create_object;
    zend_class_implements(ce TSRMLS_CC, 2, spl_ce_Countable, spl_ce_Iterator);
}

void kafka_metadata_iterator_init(zval *return_value, zval *zmetadata, const void * items, size_t item_cnt, size_t item_size, kafka_metadata_iterator_ctor_t ctor)
{
    object_intern *intern;

    if (object_init_ex(return_value, ce) != SUCCESS) {
        return;
    }

    intern = (object_intern*)zend_object_store_get_object(return_value TSRMLS_CC);
    if (!intern) {
        return;
    }

    ZVAL_ZVAL(&intern->zmetadata, zmetadata, 1, 0);
    intern->items = items;
    intern->item_cnt = item_cnt;
    intern->item_size = item_size;
    intern->ctor = ctor;
}
