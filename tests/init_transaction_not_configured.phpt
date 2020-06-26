--TEST--
initTransaction() not configured
--SKIPIF--
<?php
require __DIR__ . '/integration-tests-check.php';
if (!class_exists("RdKafka\\KafkaErrorException")) {
    echo "skip";
}
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$conf = new RdKafka\Conf();
if (RD_KAFKA_VERSION >= 0x090000 && false !== getenv('TEST_KAFKA_BROKER_VERSION')) {
    $conf->set('broker.version.fallback', getenv('TEST_KAFKA_BROKER_VERSION'));
}

$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));

$producer = new RdKafka\Producer($conf);

$producer->initTransactions(10000);
--EXPECTF--
Fatal error: Uncaught RdKafka\KafkaErrorException: _NOT_CONFIGURED in %s/tests/init_transaction_not_configured.php:13
Stack trace:
#0 %s/tests/init_transaction_not_configured.php(13): RdKafka\Producer->initTransactions(10000)
#1 {main}
  thrown in %s/tests/init_transaction_not_configured.php on line 13
