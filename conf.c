/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rdkafka.h"
#include "librdkafka/rdkafka.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "conf.h"

zend_class_entry * ce_kafka_conf;
zend_class_entry * ce_kafka_topic_conf;

static void kafka_conf_free(void *object TSRMLS_DC) /* {{{ */
{
    kafka_conf_object *intern = (kafka_conf_object*)object;

    switch (intern->type) {
        case KAFKA_CONF:
            if (intern->u.conf) {
                rd_kafka_conf_destroy(intern->u.conf);
            }
            if (intern->error_cb.fci.function_name) {
                zval_ptr_dtor(&intern->error_cb.fci.function_name);
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
    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(&intern->std, (zend_objects_store_dtor_t) zend_objects_destroy_object, kafka_conf_free, NULL TSRMLS_CC);
    retval.handlers = &kafka_object_handlers;

    return retval;    
}
/* }}} */

kafka_conf_object * get_kafka_conf_object(zval *zconf TSRMLS_DC)
{
    kafka_conf_object *oconf = (kafka_conf_object*)zend_object_store_get_object(zconf TSRMLS_CC);

    if (!oconf->type) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "RdKafka\\Conf::__construct() has not been called" TSRMLS_CC);
        return NULL;
    }

    return oconf;
}

static void kafka_conf_error_cb(rd_kafka_t *rk, int err, const char *reason, void *opaque)
{
    kafka_conf_object *intern = (kafka_conf_object*) opaque;
    zval *retval;
    zval **args[2];
    zval *zerr;
    zval *zreason;
    TSRMLS_FETCH();

    if (!intern->error_cb.fci.function_name) {
        return;
    }

    ALLOC_INIT_ZVAL(zerr);
    ZVAL_LONG(zerr, err);

    ALLOC_INIT_ZVAL(zreason);
    ZVAL_STRING(zreason, reason, 1);

    args[0] = &zerr;
    args[1] = &zreason;

    intern->error_cb.fci.retval_ptr_ptr = &retval;
    intern->error_cb.fci.params = args;
    intern->error_cb.fci.param_count = 2;

    zend_call_function(&intern->error_cb.fci, &intern->error_cb.fcc TSRMLS_CC);

    if (retval) {
        zval_ptr_dtor(&retval);
    }
    zval_ptr_dtor(&zerr);
    zval_ptr_dtor(&zreason);
}

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

    rd_kafka_conf_set_opaque(intern->u.conf, intern);

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

    intern = get_kafka_conf_object(this_ptr TSRMLS_CC);
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
        default:
            return;
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
    kafka_conf_object *intern;
    rd_kafka_conf_res_t ret = 0;
    char errstr[512];

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
        return;
    }

    intern = get_kafka_conf_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    errstr[0] = '\0';

    switch (intern->type) {
        case KAFKA_CONF:
            ret = rd_kafka_conf_set(intern->u.conf, name, value, errstr, sizeof(errstr));
            break;
        case KAFKA_TOPIC_CONF:
            ret = rd_kafka_topic_conf_set(intern->u.topic_conf, name, value, errstr, sizeof(errstr));
            break;
    }

    switch (ret) {
        case RD_KAFKA_CONF_UNKNOWN:
            zend_throw_exception(ce_kafka_exception, errstr, RD_KAFKA_CONF_UNKNOWN TSRMLS_CC);
            return;
        case RD_KAFKA_CONF_INVALID:
            zend_throw_exception(ce_kafka_exception, errstr, RD_KAFKA_CONF_INVALID TSRMLS_CC);
            return;
        case RD_KAFKA_CONF_OK:
            break;
    }
}
/* }}} */

/* {{{ proto void RdKafka\Conf::setErrorCb(mixed $callback)
   Sets the error callback */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_conf_set_error_cb, 0, 0, 1)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__Conf, setErrorCb)
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    kafka_conf_object *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f", &fci, &fcc) == FAILURE) {
        return;
    }

    intern = get_kafka_conf_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    Z_ADDREF_P(fci.function_name);

    if (intern->error_cb.fci.function_name) {
        zval_ptr_dtor(&intern->error_cb.fci.function_name);
    }

    intern->error_cb.fci = fci;
    intern->error_cb.fcc = fcc;

    rd_kafka_conf_set_error_cb(intern->u.conf, kafka_conf_error_cb);
}
/* }}} */

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

/* {{{ proto RdKafka\TopicConf::setPartitioner() */

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_topic_conf_set_partitioner, 0, 0, 1)
    ZEND_ARG_INFO(0, partitioner)
ZEND_END_ARG_INFO()

PHP_METHOD(RdKafka__TopicConf, setPartitioner)
{
    kafka_conf_object *intern;
    long id;
    int32_t (*partitioner) (const rd_kafka_topic_t * rkt, const void * keydata, size_t keylen, int32_t partition_cnt, void * rkt_opaque, void * msg_opaque);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &id) == FAILURE) {
        return;
    }

    intern = get_kafka_conf_object(this_ptr TSRMLS_CC);
    if (!intern) {
        return;
    }

    switch (id) {
        case MSG_PARTITIONER_RANDOM:
            partitioner = rd_kafka_msg_partitioner_random;
            break;
#ifdef HAVE_RD_KAFKA_MSG_PARTIIONER_CONSISTENT
        case MSG_PARTITIONER_CONSISTENT:
            partitioner = rd_kafka_msg_partitioner_consistent;
            break;
#endif
        default:
            zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Invalid partitioner" TSRMLS_CC);
            return;
    }

    rd_kafka_topic_conf_set_partitioner_cb(intern->u.topic_conf, partitioner);
}
/* }}} */

static const zend_function_entry kafka_topic_conf_fe[] = {
    PHP_ME(RdKafka__TopicConf, __construct, arginfo_kafka_conf___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, dump, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, set, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__TopicConf, setPartitioner, arginfo_kafka_topic_conf_set_partitioner, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry kafka_conf_fe[] = {
    PHP_ME(RdKafka__Conf, __construct, arginfo_kafka_conf___construct, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, dump, arginfo_kafka_conf_dump, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, set, arginfo_kafka_conf_set, ZEND_ACC_PUBLIC)
    PHP_ME(RdKafka__Conf, setErrorCb, arginfo_kafka_conf_set_error_cb, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void kafka_conf_minit(TSRMLS_D)
{
    zend_class_entry tmpce;

    INIT_NS_CLASS_ENTRY(tmpce, "RdKafka", "Conf", kafka_conf_fe);
    ce_kafka_conf = zend_register_internal_class(&tmpce TSRMLS_CC);
    ce_kafka_conf->create_object = kafka_conf_new;

    INIT_NS_CLASS_ENTRY(tmpce, "RdKafka", "TopicConf", kafka_topic_conf_fe);
    ce_kafka_topic_conf = zend_register_internal_class(&tmpce TSRMLS_CC);
    ce_kafka_topic_conf->create_object = kafka_conf_new;
}
