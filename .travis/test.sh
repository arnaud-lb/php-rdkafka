#!/bin/sh
set -xve

showmem=
if grep -q 'cfgfiles.*mem' run-tests.php; then
    echo "Will enable the --show-mem flag"
    showmem=--show-mem
fi

docker exec rdkafka-${PHP_IMAGE_TAG}-${LIBRDKAFKA_VERSION} "php run-tests.php -q -m --show-diff $showmem"
