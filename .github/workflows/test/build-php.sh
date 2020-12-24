#!/bin/sh

set -ex

if [ $MEMORY_CHECK -eq 1 ]; then
    sudo apt-get -y install valgrind
fi

if ! [ -f ~/build-cache/php/usr/local/bin/php ]; then
    echo "PHP build is not cached"

    wget https://secure.php.net/distributions/php-${PHP_VERSION}.tar.bz2
    tar xjf php-${PHP_VERSION}.tar.bz2
    cd php-${PHP_VERSION}

    PHP_BUILD_FLAGS="--prefix=/usr/local --disable-all --enable-cli --enable-cgi --with-config-file-scan-dir=/usr/local/etc/php --with-zlib"

    if [ $MEMORY_CHECK -eq 1 ]; then
        PHP_BUILD_FLAGS="$PHP_BUILD_FLAGS --enable-debug --with-valgrind"
    else
        case $PHP_VERSION in
            8.*)
                PHP_BUILD_FLAGS="$PHP_BUILD_FLAGS --enable-zts"
                ;;
            7.*)
                PHP_BUILD_FLAGS="$PHP_BUILD_FLAGS --enable-maintainer-zts"
                ;;
        esac
    fi

    ./configure $PHP_BUILD_FLAGS $PHP_BUILD_EXTRA_FLAGS
    make -j $(nproc)
    mkdir -p ~/build-cache/php
    sudo make install INSTALL_ROOT=$HOME/build-cache/php
else
    echo "PHP build is cached"
fi

sudo rsync -av ~/build-cache/php/ /
sudo mkdir -p /usr/local/etc/php
