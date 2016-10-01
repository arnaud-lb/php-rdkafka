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

#ifndef PHP_RDKAFKA_PRIV_H
#define PHP_RDKAFKA_PRIV_H

#define alloc_object(intern, ce) ecalloc(1, sizeof(*intern) + zend_object_properties_size(ce))

static inline zval * is_zval(zval * zv) {
    return zv;
}

#define get_custom_object_zval(type, zobject) \
    ((type*)((char *)Z_OBJ_P(is_zval(zobject)) - XtOffsetOf(type, std)))

static inline zend_object * is_zend_object(zend_object * object) {
    return object;
}

#define get_custom_object(type, object) \
    ((type*)((char *)is_zend_object(object) - XtOffsetOf(type, std)))

typedef struct _kafka_object kafka_object;

kafka_object * get_kafka_object(zval *zrk TSRMLS_DC);
void add_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);
void del_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);
int is_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);

#endif /* PHP_RDKAFKA_PRIV_H */
