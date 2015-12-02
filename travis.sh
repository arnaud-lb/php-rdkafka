#!/bin/sh
set -xe
git clone --depth 1 --branch 0.8.6 https://github.com/edenhill/librdkafka.git
(
    cd librdkafka
    ./configure
    make
    sudo make install
)
sudo ldconfig
phpize
./configure
make
make test
