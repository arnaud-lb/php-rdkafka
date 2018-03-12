--TEST--
RdKafka\Conf
--SKIPIF--
<?php
RD_KAFKA_VERSION >= 0x090000 || die("skip");
file_exists(__DIR__."/test_env.php") || die("skip");
--FILE--
<?php

require __DIR__."/test_env.php";

$conf = new RdKafka\Conf();

$topicConf = new RdKafka\TopicConf();
$topicConf->set('auto.offset.reset', 'smallest');

$conf->setDefaultTopicConf($topicConf);
$conf->set('metadata.broker.list', TEST_KAFKA_BROKERS);
$conf->set('group.id', sprintf("test_rdkafka_group_%s", uniqid()));

$conf->setOffsetCommitCb(function ($consumer, $error, $topicPartitions) {
    echo "Offset " . $topicPartitions[0]->getOffset() . " committed.\n";
});

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

$consumer = new RdKafka\KafkaConsumer($conf);
$consumer->subscribe([$topicName]);

while (true) {
    $msg = $consumer->consume(60 * 1000);

    if (!$msg) {
        continue;
    }

    switch ($msg->err) {
        case RD_KAFKA_RESP_ERR_NO_ERROR:
            $consumer->commit($msg);

            break;
        case RD_KAFKA_RESP_ERR__PARTITION_EOF:
            break 2;
        default:
            throw new Exception($msg->errstr());
    }
}

--EXPECT--
Offset 1 committed.
Offset 2 committed.
Offset 3 committed.
Offset 4 committed.
Offset 5 committed.
Offset 6 committed.
Offset 7 committed.
Offset 8 committed.
Offset 9 committed.
Offset 10 committed.
