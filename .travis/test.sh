#!/bin/bash
set -xve

showmem=
if grep -q 'cfgfiles.*mem' run-tests.php; then
    echo "Will enable the --show-mem flag"
    showmem=--show-mem
fi
docker run -e "TEST_PHP_EXECUTABLE=/usr/local/bin/php" rdkafka-${PHP_IMAGE_TAG}-${LIBRDKAFKA_VERSION} "php" "run-tests.php" "-q" "-m" "--show-diff" "$showmem"
