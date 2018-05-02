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

/* $Id$ */

#ifndef PHP_RDKAFKA_H
#define PHP_RDKAFKA_H

#include "librdkafka/rdkafka.h"
#include "compat.h"
#include "conf.h"

#ifndef PHP_FE_END
#define PHP_FE_END { NULL, NULL, NULL, 0, 0 }
#endif

typedef struct _kafka_object {
#if PHP_MAJOR_VERSION < 7
    zend_object             std;
#endif
    rd_kafka_type_t         type;
    rd_kafka_t              *rk;
    kafka_conf_callbacks    cbs;
    HashTable               consuming;
	HashTable				topics;
	HashTable				queues;
    unsigned short          is_persistent;
#if PHP_MAJOR_VERSION >= 7
    zend_object             std;
#endif
} kafka_object;

PHP_METHOD(RdKafka, __construct);

extern zend_module_entry rdkafka_module_entry;
#define phpext_rdkafka_ptr &rdkafka_module_entry

#define PHP_RDKAFKA_VERSION "3.0.6-dev"

extern zend_object_handlers kafka_default_object_handlers;
extern zend_class_entry * ce_kafka_exception;

#ifdef PHP_WIN32
#	define PHP_RDKAFKA_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_RDKAFKA_API __attribute__ ((visibility("default")))
#else
#	define PHP_RDKAFKA_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(rdkafka)
    HashTable kafka_instances;
ZEND_END_MODULE_GLOBALS(rdkafka)

#ifdef ZTS
#define RDKAFKA_G(v) TSRMG(rdkafka_globals_id, zend_rdkafka_globals *, v)
#else
#define RDKAFKA_G(v) (rdkafka_globals.v)
#endif

#if defined(ZTS) && defined(COMPILE_DL_RDKAFKA)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#endif	/* PHP_RDKAFKA_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
