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

#ifndef PHP_RDKAFKA_PRIV_H
#define PHP_RDKAFKA_PRIV_H

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

static inline void rdkafka_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache, zval *retval, uint32_t param_count, zval params[])
{
    int local_retval;
    zval local_retval_zv;

    if (retval) {
        local_retval = 0;
    } else {
        local_retval = 1;
        retval = &local_retval_zv;
    }

    fci->retval = retval;
    fci->params = params;
    fci->param_count = param_count;

    zend_call_function(fci, fci_cache);

    if (local_retval) {
        zval_ptr_dtor(retval);
    }
}

static inline zval *rdkafka_read_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_bool silent)
{
    zval rv;
    return zend_read_property(scope, object, name, name_length, silent, &rv);
}

static inline char *rdkafka_hash_get_current_key_ex(HashTable *ht, HashPosition *pos)
{
    zend_string* key;
    zend_ulong index;

    if (zend_hash_get_current_key_ex(ht, &key, &index, pos) == HASH_KEY_IS_STRING) {
        return key->val;
    }

    return NULL;
}

kafka_object * get_kafka_object(zval *zrk);
void add_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);
void del_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);
int is_consuming_toppar(kafka_object * intern, rd_kafka_topic_t * rkt, int32_t partition);

#endif /* PHP_RDKAFKA_PRIV_H */
