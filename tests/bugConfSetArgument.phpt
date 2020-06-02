--TEST--
RdKafka\Conf
--FILE--
<?php

class TestBug extends RdKafka\Conf
{
    public function set($name, $value)
    {
        return parent::set($name, $value);
    }
}

$conf = new TestBug();

if (RD_KAFKA_VERSION >= 0x01050001) {
    $conf->set('bootstrap.servers', '127.0.0.1');
} else {
    $conf->set('metadata.broker.list', '127.0.0.1');
}
echo "done" . PHP_EOL;
--EXPECT--
done
