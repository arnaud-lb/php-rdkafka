#!/bin/sh

echo "Building php-rdkafka with PHP version:"
php --version

if [ $MEMORY_CHECK -eq 1 ]; then
    PHP_RDKAFKA_CFLAGS="-Wall -Werror -Wno-deprecated-declarations"
fi

cd php-rdkafka
phpize
CFLAGS="$PHP_RDKAFKA_CFLAGS" ./configure
make

echo "extension=$(pwd)/modules/rdkafka.so"|sudo tee /usr/local/etc/php/rdkafka.ini >/dev/null
