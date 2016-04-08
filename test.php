<?php

error_reporting(~0);

$rd = new RdKafka\Consumer();
$rd->outQLen(false, null, 10000);

