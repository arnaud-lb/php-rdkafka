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

#include "librdkafka/rdkafka.h"

typedef struct kafka_inst_hash_pair kafka_inst_hash_pair;

struct kafka_inst_hash_pair {
    char *key;
    rd_kafka_t *value;
    struct kafka_inst_hash_pair *next;
};

typedef struct kafka_inst_hash_table {
    unsigned int size;
    kafka_inst_hash_pair **hash_pairs;
} kafka_inst_hash_table;

kafka_inst_hash_table* init_kafka_inst_hash_table(unsigned int size);
void free_kafka_inst_hash_table(kafka_inst_hash_table *h);

int has_producer_instance(kafka_inst_hash_table *hash_table, char *instance_name);
rd_kafka_t* get_persistent_producer(kafka_inst_hash_table *hash_table, char *instance_name);
void store_persistent_producer(kafka_inst_hash_table *hash_table, rd_kafka_t *rk, char *instance_name);