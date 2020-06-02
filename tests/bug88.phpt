--TEST--
Bug 88
--SKIPIF--
<?php
if (!class_exists("RdKafka\\KafkaConsumer")) {
    echo "skip";
}
--FILE--
<?php
$conf = new RdKafka\Conf();
if (RD_KAFKA_VERSION >= 0x01050001) {
    $conf->set('boostrap.servers', '127.0.0.1:9092');
} else {
    $conf->set('metadata.broker.list', '127.0.0.1:9092');
}
$consumer = new RdKafka\KafkaConsumer($conf);
echo "ok\n";
--EXPECTF--
Fatal error: Uncaught %SRdKafka\Exception%S"group.id" must be configured%s
Stack trace:
%a
