--TEST--
Can setup new callback on persistent instance
--FILE--
<?php

$conf = new RdKafka\Conf();
$conf->setDrMsgCb(function() {
    var_dump('message delivered callback 1');
});
$producer = new RdKafka\Producer($conf, 'instance');
$producer->addBrokers('kafka');
$topic = $producer->newTopic('test');
$topic->produce(RD_KAFKA_PARTITION_UA, 0, 'test message');

while($producer->getOutQLen() > 0) {
    $producer->poll(1);
}

unset($topic, $producer, $conf);

$producer = RdKafka\Producer::getInstance('instance', ['dr_msg_cb' => function(){
    var_dump('message delivered callback 2');
}]);
$producer->addBrokers('kafka');
$topic = $producer->newTopic('test');
$topic->produce(RD_KAFKA_PARTITION_UA, 0, 'test message');

while($producer->getOutQLen() > 0) {
    $producer->poll(1);
}

--EXPECT--
string(28) "message delivered callback 1"
string(28) "message delivered callback 2"