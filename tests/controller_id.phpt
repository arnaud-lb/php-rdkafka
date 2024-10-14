--TEST--
Display controller id
--SKIPIF--
<?php
require __DIR__ . '/integration-tests-check.php';
RD_KAFKA_BUILD_VERSION < 0x010000ff && die("skip librdkafka < 1.0.0");
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$conf = new RdKafka\Conf();
$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));

echo (new RdKafka\Producer($conf))->getControllerId(10*1000) . \PHP_EOL;
echo (new RdKafka\Consumer($conf))->getControllerId(10*1000) . \PHP_EOL;

$conf = new RdKafka\Conf();
$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));
$conf->set('group.id', 'test');

echo (new RdKafka\KafkaConsumer($conf))->getControllerId(10*1000) . \PHP_EOL;
--EXPECT--
1
1
1