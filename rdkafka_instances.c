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

#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "rdkafka_instances.h"

void free_next_kafka_inst_hash_pair(kafka_inst_hash_pair *hp);
void free_kafka_instance(rd_kafka_t *rk);
void insert_kafka_inst_hash_table(kafka_inst_hash_table *h, char *key, rd_kafka_t *value);
void insert_next_kafka_inst_hash_table(kafka_inst_hash_pair *hp, char* key, rd_kafka_t *value);
rd_kafka_t* search_kafka_inst_hash_table(kafka_inst_hash_table *h, char *key);
rd_kafka_t* search_chained_kafka_inst_hash_pairs(kafka_inst_hash_pair *hp, char *key);
uintptr_t kafka_inst_hash_key(char * key, unsigned int size);

kafka_inst_hash_table* init_kafka_inst_hash_table(unsigned int size)
{
    unsigned int i;

    kafka_inst_hash_table *h = pecalloc(1, sizeof(kafka_inst_hash_table), 1);
    h->hash_pairs = pecalloc(size, sizeof(kafka_inst_hash_pair*), 1);
    h->size = size;

    return h;
}

void free_kafka_inst_hash_table(kafka_inst_hash_table *h)
{
    unsigned int i;

    for (i = 0; i < h->size; i ++) {
        if (h->hash_pairs[i] != NULL) {
            if (h->hash_pairs[i]->next != NULL) {
                free_next_kafka_inst_hash_pair(h->hash_pairs[i]->next);
            }

            free_kafka_instance(h->hash_pairs[i]->value);
            pefree(h->hash_pairs[i], 1);
        }
    }
    pefree(h->hash_pairs, 1);
    pefree(h, 1);
}

void free_next_kafka_inst_hash_pair(kafka_inst_hash_pair *hp)
{
    if (hp->next != NULL){
        free_next_kafka_inst_hash_pair(hp->next);
    }

    free_kafka_instance(hp->value);
    pefree(hp, 1);
}

void free_kafka_instance(rd_kafka_t *rk)
{
    while (rd_kafka_outq_len(rk) > 0) {
        rd_kafka_poll(rk, 50);
    }
    rd_kafka_destroy(rk);

    
}

void insert_kafka_inst_hash_table(kafka_inst_hash_table *h, char *key, rd_kafka_t *value)
{
    uintptr_t index = kafka_inst_hash_key(key, h->size);
    
    if (h->hash_pairs[index] == NULL) {
        kafka_inst_hash_pair *hp = pecalloc(1, sizeof(kafka_inst_hash_pair), 1);

        hp->key = key;
        hp->value = value;
        hp->next = NULL;

        h->hash_pairs[index] = hp;
    } else {
        insert_next_kafka_inst_hash_table(h->hash_pairs[index], key, value);
    }
}

void insert_next_kafka_inst_hash_table(kafka_inst_hash_pair *hp, char* key, rd_kafka_t *value)
{
    if (hp->next == NULL) {
        kafka_inst_hash_pair *next_hp = pecalloc(1, sizeof(kafka_inst_hash_pair), 1);

        next_hp->key = key;
        next_hp->value = value;

        hp->next = next_hp;
    } else {
        insert_next_kafka_inst_hash_table(hp->next, key, value);
    }
}

uintptr_t kafka_inst_hash_key(char *key, unsigned int size)
{
    size_t len;
    unsigned int i, index;
    
    index = 0;
    len = strlen(key);

    for (i = 0; i < len; i++) {
        index += key[i];
    }

    return (uintptr_t)(index % size);
}

rd_kafka_t* search_kafka_inst_hash_table(kafka_inst_hash_table *h, char *key)
{
    uintptr_t index = kafka_inst_hash_key(key, h->size);

    if (h->hash_pairs[index] == NULL) {
        return NULL;
    }

    return search_chained_kafka_inst_hash_pairs(h->hash_pairs[index], key);
}

rd_kafka_t* search_chained_kafka_inst_hash_pairs(kafka_inst_hash_pair *hp, char *key)
{
    if (strcmp(hp->key, key) == 0) {
        return hp->value;
    }
    if (hp->next != NULL) {
        return search_chained_kafka_inst_hash_pairs(hp->next, key);
    }

    return NULL;
}

int has_producer_instance(kafka_inst_hash_table *hash_table, char *instance_name) {
    rd_kafka_t *rk = NULL;

    rk = search_kafka_inst_hash_table(hash_table, instance_name);

    if (rk != NULL) {
        return 1;
    }

    return 0;
}

rd_kafka_t* get_persistent_producer(kafka_inst_hash_table *hash_table, char *instance_name) {
    rd_kafka_t *rk = NULL;

    rk = search_kafka_inst_hash_table(hash_table, instance_name);
    
    if (rk == NULL) {
        zend_throw_exception(NULL, "Instance with given name does not exist", 0 TSRMLS_CC);
        return;
    }

    return rk;
}

void store_persistent_producer(kafka_inst_hash_table *hash_table, rd_kafka_t *rk, char *instance_name) {
    if (search_kafka_inst_hash_table(hash_table, instance_name) != NULL) {
        zend_throw_exception(NULL, "Instance with given name already exists", 0 TSRMLS_CC);
        return;
    }

    insert_kafka_inst_hash_table(hash_table, instance_name, rk);
}