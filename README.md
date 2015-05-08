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

## API

#### RdKafka\Producer

``` php
$producer = new RdKafka\Producer(RdKafka\Conf $conf = null);
```

Creates a new Kafka producer and starts its operation.

``$conf`` is an optional ``RdKafka\Conf`` instance that will
be used instead of the default configuration.
The ``$conf`` object is copied, and changing ``$conf`` after that as no effect
on the producer.
See ``RdKafka\Conf`` for more information.

#### Producer::addBrokers()

See RdKafka::addBrokers()

#### Producer::setLogLevel()

See RdKafka::setLogLevel()

#### Producer::newTopic()

``` php
$topic = $producer->newTopic(string $topic, RdKafka\TopicConf $conf);
```

Creates a new topic handle for topic named ``$topic``.

``$conf`` is an optional configuration for the topic that will be used instead
of the default topic configuration.
The ``$conf`` object is copied by this function, and changing ``$conf`` after
that has no effect on the topic.
See ``RdKafka\TopicConf`` for more information.

Returns a new ``RdKafka\ProducerTopic`` instance, or NULL on error
(see ``rd_kafka_errno()``).

#### Producer::outqLen()

``` php
$qlen = $producer->outqLen();
```

Returns the current out queue length:
messages waiting to be sent to, or acknowledged by, the broker.

#### Producer::poll()

``` php
$producer->poll(int $timeout_ms);
```

Polls the Producer handle for events.

### RdKafka

RdKafka is the base class for ``RdKafka\Producer`` and ``RdKafka\Consumer``.

#### RdKafka::addBrokers()

``` php
$rk->addBrokers(string $brokerList);
```

Adds a one or more brokers to the instance's list of initial brokers.
Additional brokers will be discovered automatically as soon as rdkafka
connects to a broker by querying the broker metadata.

If a broker name resolves to multiple addresses (and possibly
address families) all will be used for connection attempts in
round-robin fashion.

``$brokerList`` is a ,-separated list of brokers in the format:
  ``<host1>[:<port1>],<host2>[:<port2>]...``

Example:

``` php
$rk->addBrokers("10.0.0.1:9092,10.0.0.2");
```

Returns the number of brokers successfully added.

NOTE: Brokers may also be defined with the ``metadata.broker.list``
configuration property.

#### RdKafka::setLogLevel()

``` php
$rk->setLogLevel(int $level);
```

Specifies the maximum logging level produced by
internal kafka logging and debugging.
If the ``debug`` configuration property is set the level is automatically
adjusted to ``LOG_DEBUG``.

Valid values for ``$level`` are any of the syslog ``LOG_*`` priorities:
https://php.net/manual/en/function.syslog.php

### RdKafka\Conf

```
$conf = new RdKafka\Conf();
```

Creates a new configuration. The list of available configuration properties is
documented at
https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md

#### Conf::dump()

``` php
$dump = $conf->dump();
```

Dump the configuration properties and values to an array.

#### Conf::set()

```
$conf->set(string $name, string $value);
```

Sets a configuration property.

Throws a ``RdKafka\Exception`` on error.

#### RdKafka\TopicConf

``` php
$conf = new RdKafka\TopicConf();
```

Creates a new topic configuration. See ``RdKafka\Conf``.

### RdKafka\Consumer

``` php
$consumer = new RdKafka\Consumer(RdKafka\Conf $conf = null);
```

Creates a new Kafka consumer and starts its operation.

``$conf`` is an optional ``RdKafka\Conf`` instance that will
be used instead of the default configuration.
The ``$conf`` object is copied, and changing ``$conf`` after that as no effect
on the producer.
See ``RdKafka\Conf`` for more information.

#### Consumer::addBrokers()

See RdKafka::addBrokers()

#### Consumer::setLogLevel()

See RdKafka::setLogLevel()

#### Consumer::newQueue()

``` php
$queue = $consumer->newQueue();
```

Returns a RdKafka\Queue instance.

#### Consumer::newTopic()

``` php
$topic = $consumer->newTopic(string $topic, RdKafka\TopicConf $conf);
```

Creates a new topic handle for topic named ``$topic``.

``$conf`` is an optional configuration for the topic that will be used instead
of the default topic configuration.
The ``$conf`` object is copied by this function, and changing ``$conf`` after
that has no effect on the topic.
See ``RdKafka\TopicConf`` for more information.

Returns a new ``RdKafka\ConsumerTopic`` instance.

### RdKafka\ConsumerTopic

New ConsumerTopic instances can be created by calling
``RdKafka\Consumer::newTopic()``.

#### ConsumerTopic::consumeStart()

```
$topic->consumeStart(int $partition, int $offset);
```

Start consuming messages for ``$partition`` at offset ``$offset`` which may
either be a proper offset (0..N) or one of the the special offsets:  
 ``RD_KAFKA_OFFSET_BEGINNING``, ``RD_KAFKA_OFFSET_END``,
 ``RD_KAFKA_OFFSET_STORED``, ``rd_kafka_offset_tail(..)``.

rdkafka will attempt to keep ``queued.min.messages`` (config property)
messages in the local queue by repeatedly fetching batches of messages
from the broker until the threshold is reached.

The application shall use the ``consume()`` method
to consume messages from the local queue, each kafka message being
represented as a ``RdKafka\Message`` object.

``consumeStart()``must not be called multiple times for the same
topic and partition without stopping consumption first with
``consumeStop()``.

Throws a ``RdKafka\Exception`` on error.

#### ConsumerTopic::consumeStop()

``` php
$topic->consumeStop(int $partition);
```

Stop consuming messages for `$partition`, purging  all messages currently in the
local queue.

Throws a ``RdKafka\Exception`` on error.

#### ConsumerTopic::consumeQueueStart()

``` php
$topic->consumeQueueStart(int $partition, int $offset, RdKafka\Queue $queue);
```

Same as ``consumeStart()`` but re-routes incoming messages to
the provided queue ``$queue``.
The application must use one of the ``RdKafka\Queue::consume*()`` functions
to receive fetched messages.

``consumeQueueStart()`` must not be called multiple times for the
same topic and partition without stopping consumption first with
``consumeStop()``.

``consumeStart()`` and ``consumeQueueStart()`` must not be combined for the
same topic and partition.

Throws a ``RdKafka\Exception`` on error.

#### TopicConsumer::consume()

``` php
$message = $topic->consume(int $partition, int $timeout_ms);
```

Consume a single message from ``$partition``.

``$timeout_ms`` is maximum amount of time to wait for a message to be received.
Consumer must have been previously started with ``consumeStart()``.

Returns NULL on timeout.

Throws a ``RdKafka\Exception`` on error.

NOTE: The returned message's ``..->err`` must be checked for errors.
NOTE: ``..->err`` == RD_KAFKA_RESP_ERR__PARTITION_EOF' signals that the end
      of the partition has been reached, which should typically not be
      considered an error. The application should handle this case
      (e.g., ignore).

#### RdKafka\ProducerTopic

New ProducerTopic instances can be created by calling
``RdKafka\Producer::newTopic()``.

#### ProducerTopic::produce()

``` php
$topic->produce(int $partition, int $msgflags, string $payload, string $key = null)
```

Produce and send a single message to broker.

``produce()`` is an asynch non-blocking API.

``$partition`` is the target partition, either:
  - ``RD_KAFKA_PARTITION_UA`` (unassigned) for
    automatic partitioning using the topic's partitioner function, or
  - a fixed partition (0..N)

``$msgflags`` must be 0.

``$payload`` is the message payload.

``$key`` is an optional message key, if non-NULL it
will be passed to the topic partitioner as well as be sent with the
message to the broker and passed on to the consumer.

Throws a ``RdKafka\Exception`` on error.

### RdKafka\Message

A Kafka message as returned by the consuming methods.

This object has two purposes:

 * provide the application with a consumed message. (``->err`` == 0)
 * report per-topic+partition consumer errors (``->err`` != 0)

The application must check ``err`` to decide what action to take.

#### Message::$err

Non-zero for error signaling. Use ``errstr()`` for a string representation.

#### Message::$topic_name

Topic name

#### Message::$partition

Partition

#### Message::$payload

When err == 0: the message payload

#### Message::$key

When err == 0: Optional message key

#### Message::$offset

When err == 0: Message offset

#### Message::errstr()

``` php
$errstr = $message->errstr();
```

When err != 0, returns the string representation of the error.

### RdKafka\Queue

New Queue instances can be created by calling
``RdKafka\Consumer::newQueue()``.

Message queues allows the application to re-route consumed messages
from multiple topic+partitions into one single queue point.
This queue point, containing messages from a number of topic+partitions,
may then be served by a single ``consume()`` call,
rather than one per topic+partition combination.

See ``RdKafka\ConsumerTopic::consumeQueueStart()``, ``RdKafka\Queue::consume()``.

#### Queue::consume()

``` php
$message = $queue->consume(int $timeout_ms);
```

See ``RdKafka\ConsumeTopic::consume()``

### RdKafka\Exception

Exceptions thrown by php-rdkafka are of this type.




