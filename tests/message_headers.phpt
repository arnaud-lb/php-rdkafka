--TEST--
Message headers
--SKIPIF--
<?php
RD_KAFKA_VERSION >= 0x000b04ff || die("skip librdkafka too old");
require __DIR__ . '/integration-tests-check.php';
--FILE--
<?php
require __DIR__ . '/integration-tests-check.php';

$delivered = 0;

$conf = new RdKafka\Conf();
$conf->set('auto.offset.reset', 'earliest');
$conf->set('enable.partition.eof', 'true');
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

$producer = new RdKafka\Producer($conf);

if ($producer->addBrokers(getenv('TEST_KAFKA_BROKERS')) < 1) {
    echo "Failed adding brokers\n";
    exit;
}

$topicName = sprintf("test_rdkafka_%s", uniqid('', true));

$topic = $producer->newTopic($topicName);

if (!$producer->getMetadata(false, $topic, 2*1000)) {
    echo "Failed to get metadata, is broker down?\n";
}

$headers = [
    ['key' => 'value'],
    [
        'key1' => 'value1',
        'key2' => 'value2',
        'key3' => 'value3',
    ],
    [],
    null,
    ['key'],
];

foreach ($headers as $index => $header) {
    $topic->producev(0, 0, "message $index", strval($index), $header);
    $producer->poll(0);
}

while ($producer->getOutQLen()) {
    $producer->poll(50);
}

printf("%d messages delivered\n", $delivered);

$consumer = new RdKafka\Consumer($conf);
$consumer->addBrokers(getenv('TEST_KAFKA_BROKERS'));

$topic = $consumer->newTopic($topicName);
$topic->consumeStart(0, RD_KAFKA_OFFSET_BEGINNING);

$headerResults = [];

while (true) {
    $msg = $topic->consume(0, 5000);

    if ($msg->err === RD_KAFKA_RESP_ERR__PARTITION_EOF) {
        break;
    }

    if (RD_KAFKA_RESP_ERR_NO_ERROR !== $msg->err) {
        throw new Exception($msg->errstr(), $msg->err);
    }

    $headerResults[intval($msg->key)] = 'none';

    if (isset($msg->headers) && $headers[intval($msg->key)] == $msg->headers) {
        $headerResults[intval($msg->key)] = 'matched';
    }
}

foreach ($headerResults as $index => $headerMessage) {
    printf('Header for message %d | Headers: %s' . PHP_EOL, $index, $headerMessage);
}

--EXPECT--
5 messages delivered
Header for message 0 | Headers: matched
Header for message 1 | Headers: matched
Header for message 2 | Headers: none
Header for message 3 | Headers: none
Header for message 4 | Headers: none
