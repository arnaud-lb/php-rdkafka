#!/bin/sh

set -xve

if ! [ -f /home/travis/librdkafka-build/usr/local/include/librdkafka/rdkafka.h ]; then
    git clone --depth 1 --branch "${LIBRDKAFKA_VERSION:-v1.2.2}" "${LIBRDKAFKA_REPOSITORY_URL:-https://github.com/edenhill/librdkafka.git}"

    (
        cd librdkafka
        ./configure
        make
        mkdir -p /home/travis/librdkafka-build
        sudo make install DESTDIR=/home/travis/librdkafka-build
    )
fi

sudo rsync -av /home/travis/librdkafka-build/ /
sudo ldconfig

sed -i '/rdkafka/d' ~/.phpenv/versions/$(phpenv version-name)/etc/php.ini || true
echo "extension = $(pwd)/modules/rdkafka.so" >> ~/.phpenv/versions/$(phpenv version-name)/etc/php.ini
phpenv config-rm xdebug.ini || true

phpize
CFLAGS=$PHP_RDKAFKA_CFLAGS ./configure
make
