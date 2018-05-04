--TEST--
Check for existing persistent producer
--FILE--
<?php

new RdKafka\Producer(null, 'instance');
var_dump(true);
new RdKafka\Producer(null, 'instance');
--EXPECTF--
bool(true)

Fatal error: %sCannot create new producer with given name because one already exists%s
Stack trace:
%s
%s
%s