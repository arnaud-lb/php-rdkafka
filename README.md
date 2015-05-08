# php-rdkafka

PHP-rdkafka builds on top of [librdkafka](https://github.com/edenhill/librdkafka) to provide a working PHP client for [Kafka](https://kafka.apache.org/) 0.8 (and potentially old versions supported by librdkafka).

## Installation

This is a standard PHP extension:

    phpize
    ./configure
    make
    sudo make install
    # Add extension=rdkafka.so to your php.ini:
    echo extension=rdkafka.so|sudo tee -a /path/to/php.ini

## Examples

See [examples](https://github.com/arnaud-lb/php-rdkafka/tree/master/examples)
 
## Usage

### Producing

For producing, we first need to create a producer, and to add brokers (Kafka
servers) to it:

``` php
<?php

$rk = new RdKafka\Producer();
$rk->setLogLevel(LOG_DEBUG);
$rk->addBrokers("10.0.0.1,10.0.0.2");
```

Next, we create a topic instance from the producer:

```
<?php

$topic = $rk->newTopic("test");
```

From there, we can produce as much messages as we want, using the produce
method:

```
<?php

$topic->produce(RD_KAFKA_PARTITION_UA, 0, "Message payload");
```

The first argument is the partition. RD_KAFKA_PARTITION_UA stands for
*unassigned*, and lets librdkafka choose the partition.

The second argument are message flags and should always be 0, currently.

The message payload can be anything.

### Consuming

For consuming, we first need to create a consumer, and to add brokers (Kafka
servers) to it:

``` php
<?php

$rk = new RdKafka\Consumer();
$rk->setLogLevel(LOG_DEBUG);
$rk->addBrokers("10.0.0.1,10.0.0.2");
```

Next, create a topic instance by calling the `newTopic()` method, and start
consuming on partition 0:

``` php
<?php

$topic = $rk->newTopic("test");

// The first argument is the partition to consume from.
// The second argument is the offset at which to start consumption. Valid values
// are: RD_KAFKA_OFFSET_BEGINNING, RD_KAFKA_OFFSET_END, RD_KAFKA_OFFSET_STORED.
$topic->consumeStart(0, RD_KAFKA_OFFSET_BEGINNING);
```

Next, retrieve the consumed messages:

``` php
<?php

while (true) {
    // The first argument is the partition (again).
    // The second argument is the timeout.
    $msg = $topic->consume(0, 1000);
    if ($msg->err) {
        echo $msg->errstr(), "\n";
        break;
    } else {
        echo $msg->payload, "\n";
    }
}
```

### Consuming from multiple topics / partitions

Consuming from multiple topics and/or partitions can be done by telling
librdkafka to forward all messages from these topics/partitions to an internal
queue, and then consuming from this queue:

Creating the queue:

``` php
<?php
$queue = $rk->newQueue();
```

Adding topars to the queue:

``` php
<?php

$topic1 = $rk->newTopic("topic1");
$queue->consumeQueueStart(0, RD_KAFKA_OFFSET_BEGINNING, $queue);
$queue->consumeQueueStart(1, RD_KAFKA_OFFSET_BEGINNING, $queue);

$topic2 = $rk->newTopic("topic2");
$queue->consumeQueueStart(0, RD_KAFKA_OFFSET_BEGINNING, $queue);
```

Next, retrieve the consumed messages from the queue:

``` php
<?php

while (true) {
    // The only argument is the timeout.
    $msg = $queue->consume(1000);
    if ($msg->err) {
        echo $msg->errstr(), "\n";
        break;
    } else {
        echo $msg->payload, "\n";
    }
}
```

### Using stored offsets

librdkafka can store offsets in a local file, or on the broker. The default is local file, and as soon as you start using ``RD_KAFKA_OFFSET_STORED`` as consuming offset, rdkafka starts to store the offset.

By default, the file is created in the current directory, with a named based on the topic and the partition. The directory can be changed by setting the ``offset.store.path`` [configuration property](https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md).

Other interesting properties are: ``offset.store.sync.interval.ms``, ``offset.store.method``, ``auto.commit.interval.ms``, ``auto.commit.enable``, ``offset.store.method``, ``group.id``.

``` php
<?php

$topicConf = new RdKafka\TopicConf();
$topicConf->set("auto.commit.interval.ms", 1e3);
$topicConf->set("offset.store.sync.interval.ms", 60e3);

$topic = $rk->newTopic("test", $topicConf);

$topic->consumeStart(0, RD_KAFKA_OFFSET_STORED);
```

