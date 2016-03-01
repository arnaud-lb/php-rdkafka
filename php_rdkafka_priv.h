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

#define alloc_object(intern, ce) ecalloc(1, sizeof(*intern))

#define get_custom_object_zval(type, zobject) \
    ((type*)zend_object_store_get_object(zobject TSRMLS_CC))

#define get_custom_object(type, object) \
    ((type*)object)

#endif /* PHP_RDKAFKA_PRIV_H */
