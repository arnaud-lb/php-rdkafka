#!/bin/sh

set -xe

git clone --depth 1 --branch "$LIBRDKAFKA_VERSION" https://github.com/edenhill/librdkafka.git
(
    cd librdkafka
    ./configure
    make
    sudo make install
)
sudo ldconfig

sudo apt-get update
sudo apt-get install -qq valgrind

echo "extension = $(pwd)/modules/rdkafka.so" >> ~/.phpenv/versions/$(phpenv version-name)/etc/php.ini
phpenv config-rm xdebug.ini || true

phpize
./configure
make

showmem=
if grep -q 'cfgfiles.*mem' run-tests.php; then
    echo "Will enable the --show-mem flag"
    showmem=--show-mem
fi

PHP=$(which php)
REPORT_EXIT_STATUS=1 TEST_PHP_EXECUTABLE="$PHP" "$PHP" run-tests.php -q -m --show-diff $showmem
