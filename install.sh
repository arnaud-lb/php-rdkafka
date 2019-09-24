#!/bin/sh

set -xe

cd librdkafka
./configure
make
sudo make install

sudo ldconfig

phpize
CFLAGS='-Werror=implicit-function-declaration' ./configure
make

PHP=$(which php)
REPORT_EXIT_STATUS=1 TEST_PHP_EXECUTABLE="$PHP" "$PHP" run-tests.php -q -m --show-diff
