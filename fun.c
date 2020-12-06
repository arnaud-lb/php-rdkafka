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
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_get_err_descs, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_err2str, 0, 0, 1)
    ZEND_ARG_INFO(0, err)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_errno2err, 0, 0, 1)
    ZEND_ARG_INFO(0, errnox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_errno, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_thread_cnt, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kafka_offset_tail, 0, 0, 1)
    ZEND_ARG_INFO(0, cnt)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto array rd_kafka_get_err_descs()
 * Returns the full list of error codes.
 */
PHP_FUNCTION(rd_kafka_get_err_descs)
{
    const struct rd_kafka_err_desc *errdescs;
    size_t cnt;
    size_t i;
    int seen_zero = 0;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    rd_kafka_get_err_descs(&errdescs, &cnt);

    array_init_size(return_value, cnt);

    for (i = 0; i < cnt; i++) {
        const struct rd_kafka_err_desc *desc = &errdescs[i];
        zval el;

        if (desc->code == 0) {
            if (seen_zero) {
                continue;
            }
            seen_zero = 1;
        }

        ZVAL_NULL(&el);
        array_init(&el);
        add_assoc_long(&el, "code", desc->code);
        if (desc->name) {
            add_assoc_string(&el, "name", (char*) desc->name);
        } else {
            add_assoc_null(&el, "name");
        }
        if (desc->desc) {
            add_assoc_string(&el, "desc", (char*) desc->desc);
        }else {
            add_assoc_null(&el, "desc");
        }
        add_next_index_zval(return_value, &el);
    }
}
/* }}} */

/* {{{ proto string rd_kafka_err2str(int $err)
 * Returns a human readable representation of a kafka error.
 */
PHP_FUNCTION(rd_kafka_err2str)
{
    zend_long err;
    const char *errstr;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &err) == FAILURE) {
        return;
    }

    errstr = rd_kafka_err2str(err);

    if (errstr) {
        RETURN_STRING(errstr);
    }
}
/* }}} */

/* {{{ proto int rd_kafka_errno()
 * Returns `errno` */
PHP_FUNCTION(rd_kafka_errno)
{
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        return;
    }

    RETURN_LONG(errno);
}
/* }}} */

/* {{{ proto int rd_kafka_errno2err(int $errnox)
 * Converts `errno` to a rdkafka error code */
PHP_FUNCTION(rd_kafka_errno2err)
{
    zend_long errnox;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &errnox) == FAILURE) {
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
    zend_long cnt;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &cnt) == FAILURE) {
        return;
    }

    RETURN_LONG(RD_KAFKA_OFFSET_TAIL(cnt));
}
/* }}} */

/* {{{ rdkafka_functions[]
 */
const zend_function_entry rdkafka_functions[] = {
    PHP_FE(rd_kafka_get_err_descs,  arginfo_kafka_get_err_descs)
    PHP_FE(rd_kafka_err2str,        arginfo_kafka_err2str)
    PHP_DEP_FE(rd_kafka_errno2err,      arginfo_kafka_errno2err)
    PHP_DEP_FE(rd_kafka_errno,          arginfo_kafka_errno)
    PHP_FE(rd_kafka_offset_tail,    arginfo_kafka_offset_tail)
    PHP_FE(rd_kafka_thread_cnt,     arginfo_kafka_thread_cnt)
    PHP_FE_END    /* Must be the last line in rdkafka_functions[] */
};
/* }}} */

