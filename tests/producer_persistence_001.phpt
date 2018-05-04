--TEST--
Can create new persistent producer
--FILE--
<?php

$producer = new RdKafka\Producer(null, 'instance');
var_dump($producer instanceof RdKafka\Producer);
--EXPECT--
bool(true)