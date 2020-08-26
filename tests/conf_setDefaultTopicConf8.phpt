--TEST--
RdKafka\Conf::setDefaultTopicConf()
--SKIPIF--
<?php
if (!method_exists('RdKafka\Conf', 'setDefaultTopicConf') || 7 < PHP_MAJOR_VERSION) {
    echo "skip";
}
?>
--FILE--
<?php

$conf = new RdKafka\Conf();

echo "Setting valid topic conf\n";
$conf->setDefaultTopicConf(new RdKafka\TopicConf());

echo "Setting invalid topic conf\n";
$conf->setDefaultTopicConf($conf);

--EXPECTF--
Setting valid topic conf

Deprecated: Method RdKafka\Conf::setDefaultTopicConf() is deprecated in %s%econf_setDefaultTopicConf.php on line 6
Setting invalid topic conf

Deprecated: Method RdKafka\Conf::setDefaultTopicConf() is deprecated in %s%econf_setDefaultTopicConf.php on line 9

Fatal error: Uncaught TypeError: RdKafka\Conf::setDefaultTopicConf(): Argument #1 ($topic_conf) must be of type RdKafka\TopicConf, RdKafka\Conf given in %s%econf_setDefaultTopicConf.php on line 9

