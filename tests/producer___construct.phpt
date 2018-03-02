--TEST--
Check for existing persistent producer
--FILE--
<?php

new RdKafka\Producer(null, 'instance');
var_dump(true);
new RdKafka\Producer(null, 'instance');
--EXPECTF--
bool(true)

Fatal error: Uncaught Exception: Cannot create new producer with given name because one already exists. Use RdKafka\Producer::getInstance() to retrive it. %s
Stack trace:
%s
%s
%s