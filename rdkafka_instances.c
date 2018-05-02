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

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_hash.h"
#include "php_rdkafka.h"
#include "php_rdkafka_priv.h"

ZEND_DECLARE_MODULE_GLOBALS(rdkafka)

zend_bool has_producer_instance(char *instance_name, arglen_t instance_name_len) {
    return zend_hash_str_exists(&RDKAFKA_G(kafka_instances), instance_name, instance_name_len);
}

rd_kafka_t* get_persistent_producer(char *instance_name, arglen_t instance_name_len) {
    rd_kafka_t *rk = NULL;

    rk = zend_hash_str_find_ptr(&RDKAFKA_G(kafka_instances), instance_name, instance_name_len);
    if (rk == NULL) {
        zend_throw_exception(NULL, "Instance with given name does not exist", 0 TSRMLS_CC);
        return NULL;
    }
    
    return rk;
}

void store_persistent_producer(rd_kafka_t *rk, char *instance_name, arglen_t instance_name_len) {
    zend_hash_str_add_ptr(&RDKAFKA_G(kafka_instances), instance_name, instance_name_len, rk);
}