--TEST--
Bug 115
--SKIPIF--
<?php
require __DIR__ . '/integration-tests-check.php';
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$delivered = 0;

$conf = new RdKafka\Conf();
if (RD_KAFKA_VERSION >= 0x090000 && false !== getenv('TEST_KAFKA_BROKER_VERSION')) {
    $conf->set('broker.version.fallback', getenv('TEST_KAFKA_BROKER_VERSION'));
}
$conf->setErrorCb(function ($producer, $err, $errstr) {
    printf("%s: %s\n", rd_kafka_err2str($err), $errstr);
    exit;
});
$conf->setDrMsgCb(function ($producer, $msg) use (&$delivered) {
    if ($msg->err) {
        throw new Exception("Message delivery failed: " . $msg->errstr());
    }
    $delivered++;
});
$conf->set('metadata.broker.list', getenv('TEST_KAFKA_BROKERS'));

$topicName = sprintf("test_rdkafka_%s", uniqid());

$consumer = new RdKafka\Consumer($conf);

$topic = $consumer->newTopic($topicName);
$topic->consumeStart(0, RD_KAFKA_OFFSET_BEGINNING);

while (true) {
    $msg = $topic->consume(0, 1000);
    // librdkafka before 1.0 returns message with RD_KAFKA_RESP_ERR__PARTITION_EOF when reaching topic end.
    if (!$msg || RD_KAFKA_RESP_ERR__PARTITION_EOF === $msg->err) {
        break;
    }
}

$topic->consumeStop(0);
--EXPECT--
