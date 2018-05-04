--TEST--
Can reuse persistent producer to send a message
--FILE--
<?php

$conf = new RdKafka\Conf();
$conf->setDrMsgCb(function() {
    var_dump('message delivered');
});
$producer = new RdKafka\Producer($conf, 'instance');
$producer->addBrokers('kafka');
$topic = $producer->newTopic('test');
$topic->produce(RD_KAFKA_PARTITION_UA, 0, 'test message');

while($producer->getOutQLen() > 0) {
    $producer->poll(1);
}

unset($topic, $producer, $conf);

$producer = RdKafka\Producer::getInstance('instance');
$producer->addBrokers('kafka');
$topic = $producer->newTopic('test');
$topic->produce(RD_KAFKA_PARTITION_UA, 0, 'test message');

while($producer->getOutQLen() > 0) {
    $producer->poll(1);
}
var_dump(true);
--EXPECT--
string(17) "message delivered"
bool(true)