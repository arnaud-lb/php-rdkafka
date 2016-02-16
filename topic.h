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

typedef struct _kafka_topic_object {
    zend_object         std;
    rd_kafka_topic_t    *rkt;
    zval                *zrk;
} kafka_topic_object;

void kafka_topic_minit(TSRMLS_D);
kafka_topic_object * get_kafka_topic_object(zval *zrkt TSRMLS_DC);

extern zend_class_entry * ce_kafka_consumer_topic;
extern zend_class_entry * ce_kafka_producer_topic;
extern zend_class_entry * ce_kafka_topic;
