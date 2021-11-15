--TEST--
Bug #465
--SKIPIF--
<?php
require __DIR__ . '/integration-tests-check.php';
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$n = 0;

foreach (getTopicS() as $topicMetadata) {
    $n++;
    $topicMetadata->getTopic();
}

var_dump($n > 0);

function getTopics() {
    $conf = new RdKafka\Conf();
    if (RD_KAFKA_VERSION >= 0x090000 && false !== getenv('TEST_KAFKA_BROKER_VERSION')) {
        $conf->set('broker.version.fallback', getenv('TEST_KAFKA_BROKER_VERSION'));
    }
    $conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));

    $consumer = new RdKafka\Consumer($conf);

    $topicName = sprintf("test_rdkafka_%s", uniqid());

    $consumer->newTopic($topicName);

    return $consumer->getMetadata(true, null, 2*1000)->getTopics();
}
--EXPECT--
bool(true)
