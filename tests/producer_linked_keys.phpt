--TEST--
Test linked keys in hash table
--FILE--
<?php

// All the following names are producing the same hash effectively storing instances ina linked list
$producer = new RdKafka\Producer(null, 'my_instance_test');
$producer2 = new RdKafka\Producer(null, 'my_test_instance');
$producer3 = new RdKafka\Producer(null, 'instance_my_test');
$producer4 = new RdKafka\Producer(null, 'test_instance_my');

var_dump(RdKafka\Producer::getInstance('my_instance_test') instanceof RdKafka\Producer);
var_dump(RdKafka\Producer::getInstance('my_test_instance') instanceof RdKafka\Producer);
var_dump(RdKafka\Producer::getInstance('instance_my_test') instanceof RdKafka\Producer);
var_dump(RdKafka\Producer::getInstance('test_instance_my') instanceof RdKafka\Producer);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)