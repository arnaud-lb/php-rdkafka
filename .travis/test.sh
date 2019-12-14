#!/bin/bash
set -xve

if grep -q 'cfgfiles.*mem' ${TRAVIS_BUILD_DIR}/run-tests.php; then
    echo "Will enable the --show-mem flag"
    exprt SHOWMEM=--show-mem
fi

docker run \
  --network="host" \
  -e TEST_KAFKA_BROKERS=${TEST_KAFKA_BROKERS} \
  -e TEST_KAFKA_BROKER_VERSION=${TEST_KAFKA_BROKER_VERSION} \
  -e "TEST_PHP_EXECUTABLE=/usr/local/bin/php" \
  -e "REPORT_EXIT_STATUS=1" \
  rdkafka-${PHP_IMAGE_TAG}-${LIBRDKAFKA_VERSION} \
  "php" "run-tests.php" "-q" "-m" "--show-diff" "${SHOWMEM}"

exit $?
