--TEST--
RdKafka\Conf
--SKIPIF--
<?php
RD_KAFKA_VERSION >= 0x090000 || die("skip librdkafka too old");
require __DIR__ . '/integration-tests-check.php';
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$conf = new RdKafka\Conf();

$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));
$conf->set('group.id', sprintf("test_rdkafka_group_%s", uniqid()));

$producer = new RdKafka\Producer($conf);

$topicName = sprintf("test_rdkafka_%s", uniqid());
$topic = $producer->newTopic($topicName);

for ($i = 0; $i < 10; $i++) {
    $topic->produce(0, 0, "message $i");
    $producer->poll(0);
}

while ($producer->getOutQLen()) {
    $producer->poll(50);
}

$conf = new RdKafka\Conf();

$conf->set('auto.offset.reset', 'earliest');
$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));
$conf->set('group.id', sprintf("test_rdkafka_group_%s", uniqid()));
$conf->set('enable.partition.eof', 'false');

$consumer = new RdKafka\KafkaConsumer($conf);
$consumer->subscribe([$topicName]);

$listeningEnd = time() + 15;
$i = 0;
while ($i < 10 && time() < $listeningEnd) {
    $msg = $consumer->consume(500);

    if ($msg === null) {
        continue;
    }

    if (RD_KAFKA_RESP_ERR_NO_ERROR !== $msg->err) {
        throw new Exception($msg->errstr(), $msg->err);
    }

    echo $msg->payload, "\n";
    $i++;

    $consumer->commit($msg);
}

--EXPECT--
message 0
message 1
message 2
message 3
message 4
message 5
message 6
message 7
message 8
message 9
