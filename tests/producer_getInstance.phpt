--TEST--
Create and retrive persistent producer
--FILE--
<?php

$false = RdKafka\Producer::getInstance('instance');


$producer = new RdKafka\Producer(null, 'instance');
$sameProducer = RdKafka\Producer::getInstance('instance');

var_dump($false);
var_dump($producer instanceof RdKafka\Producer);
var_dump($sameProducer instanceof RdKafka\Producer);
--EXPECT--
bool(false)
bool(true)
bool(true)