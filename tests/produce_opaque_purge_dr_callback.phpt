--TEST--
Produce with opaque, purge queued/inflight messages, with delivery callback
--SKIPIF--
<?php
require __DIR__ . '/integration-tests-check.php';
RD_KAFKA_BUILD_VERSION < 0x1000000 && die("skip librdkafka < 1.0.0");
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$conf = new RdKafka\Conf();
if (RD_KAFKA_VERSION >= 0x090000 && false !== getenv('TEST_KAFKA_BROKER_VERSION')) {
    $conf->set('broker.version.fallback', getenv('TEST_KAFKA_BROKER_VERSION'));
}
$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));

$conf->setDrMsgCb(function ($rdkafka, $msg) {
    var_dump($rdkafka, $msg);
});

$producer = new RdKafka\Producer($conf);

$topicName = sprintf("test_rdkafka_%s", uniqid());

$topic = $producer->newTopic($topicName);

if (!$producer->getMetadata(false, $topic, 10*1000)) {
    echo "Failed to get metadata, is broker down?\n";
}

for ($i = 0; $i < 10; $i++) {
    $topic->produce(0, 0, "message $i", null, "opaque $i");
}

$producer->purge(RD_KAFKA_PURGE_F_QUEUE | RD_KAFKA_PURGE_F_INFLIGHT);

echo "Expect no leaks\n";
--EXPECT--
Expect no leaks
