#!/bin/sh

set -e

sudo ldconfig

echo "extension = $(pwd)/modules/rdkafka.so" >> ~/.phpenv/versions/$(phpenv version-name)/etc/php.ini
phpenv config-rm xdebug.ini || true

phpize
CFLAGS='-Werror=implicit-function-declaration' ./configure
make
