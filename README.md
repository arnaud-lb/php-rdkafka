# php-rdkafka

[![Join the chat at https://gitter.im/arnaud-lb/php-rdkafka](https://badges.gitter.im/arnaud-lb/php-rdkafka.svg)](https://gitter.im/arnaud-lb/php-rdkafka?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![Build Status](https://travis-ci.org/arnaud-lb/php-rdkafka.svg)](https://travis-ci.org/arnaud-lb/php-rdkafka)

PHP-rdkafka is a thin [librdkafka](https://github.com/edenhill/librdkafka) binding providing a working PHP 5 / PHP 7 [Kafka](https://kafka.apache.org/) 0.8 / 0.9 client.

It supports the *consumer*, *producer*, and *metadata* APIs.

The API ressembles as much as possible to librdkafka's.

## Table of Contents

1. [Installation](#installation)
   * [Dependencies](#dependencies)
   * [PHP 5 / PHP 7](#php-5--php-7)
   * [Using PECL](#using-pecl)
   * [OSX / Homebrew](#osx--homebrew)
   * [From source](#from-source)
2. [Examples](#examples)
3. [Usage](#usage)
   * [Producing](#producing)
   * [Consuming](#consuming)
   * [Consuming form multiple topics / partitions](#consuming-from-multiple-topics--partitions)
   * [Using stored offsets](#using-stored-offsets)
   * [Interesting configuration parameters](#interesting-configuration-parameters)
     * [queued.max.messages.kbytes](#queuedmaxmessageskbytes)
     * [topic.metadata.refresh.sparse and topic.metadata.refresh.interval.ms](#topicmetadatarefreshsparse-and-topicmetadatarefreshintervalms)
     * [internal.termination.signal](#internalterminationsignal)
4. [API](#api)
   * [RdKafka\Consumer](#rdkafkaconsumer)
     * [Consumer::addBrokers()](#consumeraddbrokers)
     * [Consumer::setLogLevel()](#consumersetloglevel)
     * [Consumer::metadata()](#consumermetadata)
     * [Consumer::newQueue()](#consumernewqueue)
     * [Consumer::newTopic()](#consumernewtopic)
   * [RdKafka\Producer](#rdkafkaproducer)
     * [Producer::addBrokers()](#produceraddbrokers)
     * [Producer::setLogLevel()](#producersetloglevel)
     * [Producer::metadata()](#producermetadata)
     * [Producer::newTopic()](#producernewtopic)
     * [Producer::outqLen()](#produceroutqlen)
     * [Producer::poll()](#producerpoll)
   * [RdKafka](#rdkafka)
     * [RdKafka::addBrokers()](#rdkafkaaddbrokers)
     * [RdKafka::setLogLevel()](#rdkafkasetloglevel)
     * [RdKafka::metadata()](#rdkafkametadata)
   * [RdKafka\Conf](#rdkafkaconf)
     * [Conf::dump()](#confdump)
     * [Conf::set()](#confset)
   * [RdKafka\TopicConf](#rdkafkatopicconf)
     * [TopicConf::setPartitioner()](#topicconfsetpartitioner)
   * [RdKafka\Topic](#rdkafkatopic)
     * [Topic::getName()](#topicgetname)
   * [RdKafka\ConsumerTopic](#rdkafkaconsumertopic)
     * [ConsumerTopic::consumeStart()](#consumertopicconsumestart)
     * [ConsumerTopic::consumeStop()](#consumertopicconsumestop)
     * [ConsumerTopic::consumeQueueStart()](#consumertopicconsumequeuestart)
     * [ConsumerTopic::consume()](#consumertopicconsume)
     * [ConsumerTopic::getName()](#consumertopicgetname)]
     * [ConsumerTopic::offsetStore()](#consumertopicoffsetstore)
   * [RdKafka\ProducerTopic](#rdkafkaproducertopic)
     * [ConsumerTopic::getName()](#producertopicgetname)]
     * [ProducerTopic::produce()](#producertopicproduce)
   * [RdKafka\Message](#rdkafkamessage)
     * [Message::$err](#messageerr)
     * [Message::$topic_name](#messagetopic_name)
     * [Message::$partition](#messagepartition)
     * [Message::$payload](#messagepayload)
     * [Message::$key](#messagekey)
     * [Message::$offset](#messageoffset)
     * [Message::errstr()](#messageerrstr)
   * [RdKafka\Queue](#rdkafkaqueue)
     * [Queue::consume()](#queueconsume)
   * [RdKafka\Exception](#rdkafkaexception)
   * [RdKafka\Metadata](#rdkafkametadata-1)
     * [Metadata::getOrigBrokerId()](#metadatagetorigbrokerid)
     * [Metadata::getOrigBrokerName()](#metadatagetorigbrokerid)
     * [Metadata::getBrokers()](#metadatagetbrokers)
     * [Metadata::getTopics()](#metadatagettopics)
   * [RdKafka\Metadata\Topic](#rdkafkametadatatopic)
     * [Topic::getTopic()](#topicgettopic)
     * [Topic::getErr()](#topicgeterr)
     * [Topic::getPartitions()](#topicgetpartitions)
   * [RdKafka\Metadata\Broker](#rdkafkametadatabroker)
     * [Broker::getId()](#brokergetid)
     * [Broker::getHost()](#brokergethost)
     * [Broker::getPort()](#brokergetport)
   * [RdKafka\Metadata\Partition](#rdkafkametadatapartition)
     * [Partition::getId()](#partitiongetid)
     * [Partition::getErr()](#partitiongeterr)
     * [Partition::getLeader()](#partitiongetleader)
     * [Partition::getReplicas()](#partitiongetreplicas)
     * [Partition::getIsrs()](#partitiongetisrs)
   * [RdKafka\Metadata\Collection](#rdkafkametadatacollection)
   * [Functions](#functions)
     * [rd_kafka_err2str](#rd_kafka_err2str)
     * [rd_kafka_errno2err](#rd_kafka_errno2err)
     * [rd_kafka_errno](#rd_kafka_errno)
     * [rd_kafka_offset_tail](#rd_kafka_offset_tail)
   * [Constants](#constants)
5. [Credits](#credits)
6. [License](#license)

## Installation

### Dependencies

php-rdkafka depends on the stable version of [librdkafka](https://github.com/edenhill/librdkafka/releases)

### PHP 5 / PHP 7

php-rdkafka is compatible with PHP 5 (master branch, PECL release); and has an experimental [PHP 7 branch](https://github.com/arnaud-lb/php-rdkafka/tree/php7)

### Using PECL

For PHP version 7, installation from source should be preferred.

    sudo pecl install channel://pecl.php.net/rdkafka-alpha

### OSX / Homebrew

    brew tap homebrew/dupes
    brew tap homebrew/versions
    brew tap homebrew/homebrew-php
    brew install homebrew/php/php70-rdkafka

`php70-rdkafka` is the rdkafka package for PHP 7.0. Replace `70` by 53, 54, 55, or 56 for PHP version 5.3, 5.4, 5.5, or 5.6, respectively.

### From source

For PHP version 7, make sure to use the php7 branch.

    git clone https://github.com/arnaud-lb/php-rdkafka.git
    cd php-rdkafka
    # for php7 only:
    # git checkout php7
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
$topic1->consumeQueueStart(0, RD_KAFKA_OFFSET_BEGINNING, $queue);
$topic1->consumeQueueStart(1, RD_KAFKA_OFFSET_BEGINNING, $queue);

$topic2 = $rk->newTopic("topic2");
$topic2->consumeQueueStart(0, RD_KAFKA_OFFSET_BEGINNING, $queue);
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

### Interesting configuration parameters

#### queued.max.messages.kbytes

librdkafka will buffer up to 1GB of messages for each consumed partition by default. You can lower memory usage by reducing the value of the ``queued.max.messages.kbytes`` parameter on your consumers.

### topic.metadata.refresh.sparse and topic.metadata.refresh.interval.ms

Each consumer and procuder instance will fetch topics metadata at an interval defined by the ``topic.metadata.refresh.interval.ms`` parameter. Depending on your librdkafka version, the parameter defaults to 10 seconds, or 600 seconds.

librdkafka fetches the metadata for all topics of the cluster by default. Setting ``topic.metadata.refresh.sparse`` to the string ``"true"`` makes sure that librdkafka fetches only the topics he uses.

Setting ``topic.metadata.refresh.sparse`` to ``"true"``, and ``topic.metadata.refresh.interval.ms`` to 600 seconds (plus some jitter) can reduce the bandwidth a lot, depending on the number of consumers and topics.

### internal.termination.signal

This setting allows librdkafka threads to terminate as soon as librdkafka is done with them. This effectively allows your PHP processes / requests to terminate quickly.

When enabling this, you have to mask the signal like this:

``` php
<?php
// once
pcntl_sigprocmask(SIG_BLOCK, array(SIGIO));
// any time
$conf->set('internal.termination.signal', SIGIO);
```

## API

### RdKafka\Producer

``` php
$producer = new RdKafka\Producer(RdKafka\Conf $conf = null);
```

Creates a new Kafka producer and starts its operation.

``$conf`` is an optional [``RdKafka\Conf``](#rdkafkaconf) instance that will
be used instead of the default configuration.
The ``$conf`` object is copied, and changing ``$conf`` after that as no effect
on the producer.
See  [``RdKafka\Conf``](#rdkafkaconf) for more information.

#### Producer::addBrokers()

See [RdKafka::addBrokers()](#rdkafkaaddbrokers)

#### Producer::setLogLevel()

See [RdKafka::setLogLevel()](#rdkafkasetloglevel)

#### Producer::metadata()

See [RdKafka::metadata()](#rdkafkametadata)

#### Producer::newTopic()

``` php
$topic = $producer->newTopic(string $topic, RdKafka\TopicConf $conf = null);
```

Creates a new [``RdKafka\ProducerTopic``](#rdkafkatopicconf) instance for topic named ``$topic``.

``$conf`` is an optional configuration for the topic that will be used instead
of the default topic configuration.
The ``$conf`` object is copied by this function, and changing ``$conf`` after
that has no effect on the topic.
See [``RdKafka\TopicConf``](#rdkafkatopicconf) for more information.

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

### RdKafka\Consumer

``` php
$consumer = new RdKafka\Consumer(RdKafka\Conf $conf = null);
```

Creates a new Kafka consumer and starts its operation.

``$conf`` is an optional [``RdKafka\Conf``](#rdkafkaconf) instance that will
be used instead of the default configuration.
The ``$conf`` object is copied, and changing ``$conf`` after that as no effect
on the producer.
See [``RdKafka\Conf``](#rdkafkaconf) for more information.

#### Consumer::addBrokers()

See [RdKafka::addBrokers()](#rdkafkaaddbrokers)

#### Consumer::setLogLevel()

See [RdKafka::setLogLevel()](#rdkafkasetloglevel)

#### Consumer::metadata()

See [RdKafka::metadata()](#rdkafkametadata)

#### Consumer::newQueue()

``` php
$queue = $consumer->newQueue();
```

Returns a [RdKafka\Queue](#rdkafkaqueue) instance.

#### Consumer::newTopic()

``` php
$topic = $consumer->newTopic(string $topic, RdKafka\TopicConf $conf = null);
```

Creates a new [``RdKafka\ConsumerTopic``](#rdkafkaconsumertopic) for topic named ``$topic``.

``$conf`` is an optional configuration for the topic that will be used instead
of the default topic configuration.
The ``$conf`` object is copied by this function, and changing ``$conf`` after
that has no effect on the topic.
See [``RdKafka\TopicConf``](#rdkafkatopicconf) for more information.

### RdKafka

RdKafka is the base class for [``RdKafka\Producer``](#rdkafkaproducer) and [``RdKafka\Consumer``](#rdkafkaconsumer).

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

#### RdKafka::metadata()

```
$metadata = $rk->metadata(bool $all_topics, RdKafka\Topic $only_topic = null, int $timeout_ms);
```

Request Metadata from broker.

 * all_topics - if true: request info about all topics in cluster,
             if false: only request info about locally known topics.
 * only_rkt   - only request info about this topic
 * timeout_ms - maximum response time before failing.

Returns a [`RdKafka\Metadata`](#rdkafkametadata-1)

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

### RdKafka\TopicConf

``` php
$conf = new RdKafka\TopicConf();
```

Creates a new topic configuration. See [``RdKafka\Conf``](#rdkafkaconf).

#### TopicConf::setPartitioner()

Set partitioner callback.

Allowed values are [``RD_KAFKA_MSG_PARTITIONER_RANDOM``](#rd_kafka_msg_partitioner_random),
[``RD_KAFKA_MSG_PARTITIONER_CONSISTENT``](#rd_kafka_msg_partitioner_consistent).

### RdKafka\Topic

RdKafka\Topic is the base class for [``RdKafka\ConsumerTopic``](#rdkafkaconsumertopic) and [``RdKafka\ProducerTopic``](#rdkafkaproducertopic).

#### Topic::getName()

``` php
$name = $topic->getName();
```

Returns the topic name.

### RdKafka\ConsumerTopic

New ConsumerTopic instances can be created by calling
[``RdKafka\Consumer::newTopic()``](#consumernewtopic).

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

The application shall use the [``consume()``](#consumertopicconsume) method
to consume messages from the local queue, each kafka message being
represented as a [``RdKafka\Message``](#rdkafkamessage) object.

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
The application must use one of the [``RdKafka\Queue::consume*()``](#queueconsume) functions
to receive fetched messages.

``consumeQueueStart()`` must not be called multiple times for the
same topic and partition without stopping consumption first with
``consumeStop()``.

``consumeStart()`` and ``consumeQueueStart()`` must not be combined for the
same topic and partition.

Throws a ``RdKafka\Exception`` on error.

#### ConsumerTopic::consume()

``` php
$message = $topic->consume(int $partition, int $timeout_ms);
```

Consume a single message from ``$partition``.

``$timeout_ms`` is maximum amount of time to wait for a message to be received.
Consumer must have been previously started with [``consumeStart()``](#consumetopicconsumestart).

Returns NULL on timeout.

Throws a ``RdKafka\Exception`` on error.

NOTE: The returned message's [``..->err``](#messageerr) must be checked for errors.  
NOTE: [``..->err``](#messageerr) == ``RD_KAFKA_RESP_ERR__PARTITION_EOF`` signals that the end
      of the partition has been reached, which should typically not be
      considered an error. The application should handle this case
      (e.g., ignore).

### ConsumerTopic::getName()

See [Topic::getName()](#topicgetname)

### ConsumerTopic::offsetStore()

``` php
$topic->offsetStore($message->partition, $message->offset+1);
```

Store offset ``offset`` for topic ``rkt`` partition ``partition``.  The
offset will be commited (written) to the offset store according to
``auto.commit.interval.ms``.

NOTE: ``auto.commit.enable`` must be set to ``"false"`` when using this API.

Throws a ``RdKafka\Exception`` on error.

### RdKafka\ProducerTopic

New ProducerTopic instances can be created by calling
[``RdKafka\Producer::newTopic()``](#producernewtopic).

### ProducerTopic::getName()

See [Topic::getName()](#topicgetname)

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

 * provide the application with a consumed message. ([``->err``](#messageerr) == 0)
 * report per-topic+partition consumer errors ([``->err``](#messageerr) != 0)

The application must check [``err``](#messageerr) to decide what action to take.

#### Message::$err

Non-zero for error signaling. Use [``errstr()``](#messageerrstr) for a string representation.

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
[``RdKafka\Consumer::newQueue()``](#consumernewqueue).

Message queues allows the application to re-route consumed messages
from multiple topic+partitions into one single queue point.
This queue point, containing messages from a number of topic+partitions,
may then be served by a single [``consume()``](#queueconsume) call,
rather than one per topic+partition combination.

See [``RdKafka\ConsumerTopic::consumeQueueStart()``](#consumetopicconsumequeuestart), [``RdKafka\Queue::consume()``](#queueconsume).

#### Queue::consume()

``` php
$message = $queue->consume(int $timeout_ms);
```

See [``RdKafka\ConsumerTopic::consume()``](#consumertopicconsume)

### RdKafka\Exception

Exceptions thrown by php-rdkafka are of this type.

### RdKafka\Metadata

Metadata container.

See [``RdKafka::metadata()``](#rdkafkametadata).

#### Metadata::getOrigBrokerId()

``` php
$id = $metadata->getOrigBrokerId();
```

Returns the broker originating this metadata.

#### Metadata::getOrigBrokerName()

``` php
$name = $metadata->getOrigBrokerName();
```

Returns the name of originating broker.

#### Metadata::getBrokers()

``` php
$brokers = metadata->getBrokers();

printf("There are %d brokers", count($brokers));

foreach ($brokers as $broker) {
    ...
}
```

Returns a [`RdKafka\Metadata\Collection`](#rdkafkametadatacollection) of [`RdKafka\Metadata\Broker`](#rdkafkametadatabroker).

#### Metadata::getTopics()

``` php
$topics = $metadata->getTopics();

printf("There are %d topics", count($topics));

foreach ($topics as $topic) {
    ...
}
```

Returns a [`RdKafka\Metadata\Collection`](#rdkafkametadatacollection) of [`RdKafka\Metadata\Topic`](#rdkafkametadatatopic).

### RdKafka\Metadata\Broker

Metadata: Broker information.

See [``Metadata::getBrokers()``](#metadatagetbrokers).

#### Broker::getId()

``` php
$id = $broker->getId();
```

Returns the broker id.

#### Broker::getHost()

``` php
$host = $broker->getHost();
```

Returns the broker hostname.

#### Broker::getPort()

``` php
$port = $broker->getPort();
```

Returns the broker port.

### RdKafka\Metadata\Topic

Metadata: Topic information.

See [``Metadata::getTopics()``](#metadatagettopics).

#### Topic::getTopic()

``` php
$name = $topic->getTopic();
```

Returns the topic name.

#### Topic::getErr()

``` php
$name = $topic->getErr();
```

Returns the topic error reported by broker.

#### Topic::getPartitions()

``` php
$topics = $topic->getPartitions();

printf("There are %d partitions", count($partitions));

foreach ($partitions as $partition) {
    ...
}
```

Returns a [`RdKafka\Metadata\Collection`](#rdkafkametadatacollection) of [`RdKafka\Metadata\Partition`](#rdkafkametadatapartition).

### RdKafka\Metadata\Partition

Metadata: Partition information.

See [``Topic::getPartitions()``](#topicgetpartitions).

#### Partition::getId()

``` php
$id = $partition->getId();
```

Returns the partition id.

#### Partition::getErr()

``` php
$err = $partition->getErr();
```

Returns the partition error reported by broker.

#### Partition::getLeader()

``` php
$leader = $partition->getLeader();
```

Returns the leader broker id.

#### Partition::getReplicas()

``` php
$replicas = $partitions->getReplicas();

printf("There are %d replicas", count($replicas));

foreach ($replicas as $replica) {
    ...
}
```

Returns a [`RdKafka\Metadata\Collection`](#rdkafkametadatacollection) of replica broker ids for this partition.

#### Partition::getIsrs()

``` php
$replicas = $partitions->getIsrs();

printf("There are %d In-Sync-Replicas", count($replicas));

foreach ($replicas as $replica) {
    ...
}
```

Returns a [`RdKafka\Metadata\Collection`](#rdkafkametadatacollection) of In-Sync-Replica broker ids for this partition.

### RdKafka\Metadata\Collection

`RdKafka\Metadata\Collection` implements [`Iterator`](https://php.net/manual/en/class.iterator.php) (can be used in `foreach`), and [`Countable`](https://php.net/manual/en/class.countable.php) (can be used in `count()`).

### Functions

#### rd_kafka_err2str()

Returns a human readable representation of a kafka error

#### rd_kafka_errno2err()

Converts `errno` to a `rd_kafka_resp_err_t` error code

#### rd_kafka_errno()

Returns `errno`

#### rd_kafka_offset_tail()

``` php
$offset = rd_kafka_offset_tail($cnt);
```

Returns a special offset to start consuming ``$cnt`` messages from topic's current ``.._END`` offset.
That is, if current end offset is 12345 and ``$cnt`` is 200, it will start consuming from offset 12345-200 = 12145.

### Constants

#### RD_KAFKA_OFFSET_BEGINNING

Start consuming from beginning of kafka partition queue: oldest msg

#### RD_KAFKA_OFFSET_END

Start consuming from end of kafka partition queue: next msg

#### RD_KAFKA_OFFSET_STORED

Start consuming from offset retrieved from offset store

#### RD_KAFKA_PARTITION_UA

Unassigned partition.

The unassigned partition is used by the producer API for messages
that should be partitioned using the configured or default partitioner.

#### RD_KAFKA_VERSION

librdkafka version

Interpreted as hex MM.mm.rr.xx:

 * MM = Major
 * mm = minor
 * rr = revision
 * xx = currently unused

I.e.: 0x00080100 = 0.8.1

#### RD_KAFKA_RESP_ERR__BEGIN

begin internal error codes

#### RD_KAFKA_RESP_ERR__BAD_MSG

Received message is incorrect

#### RD_KAFKA_RESP_ERR__BAD_COMPRESSION

Bad/unknown compression

#### RD_KAFKA_RESP_ERR__DESTROY

Broker is going away

#### RD_KAFKA_RESP_ERR__FAIL

Generic failure

#### RD_KAFKA_RESP_ERR__TRANSPORT

Broker transport error

#### RD_KAFKA_RESP_ERR__CRIT_SYS_RESOURCE

Critical system resource failure

#### RD_KAFKA_RESP_ERR__RESOLVE

Failed to resolve broker

#### RD_KAFKA_RESP_ERR__MSG_TIMED_OUT

Produced message timed out

#### RD_KAFKA_RESP_ERR__PARTITION_EOF

Reached the end of the topic+partition queue on the broker. Not really an error.

#### RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION

Permanent: Partition does not exist in cluster.

#### RD_KAFKA_RESP_ERR__FS

File or filesystem error

#### RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC

Permanent: Topic does not exist in cluster.

#### RD_KAFKA_RESP_ERR__ALL_BROKERS_DOWN

All broker connections are down.

#### RD_KAFKA_RESP_ERR__INVALID_ARG

Invalid argument, or invalid configuration

#### RD_KAFKA_RESP_ERR__TIMED_OUT

Operation timed out

#### RD_KAFKA_RESP_ERR__QUEUE_FULL

Queue is full

#### RD_KAFKA_RESP_ERR__ISR_INSUFF

ISR count < required.acks

#### RD_KAFKA_RESP_ERR__END

end internal error codes

#### RD_KAFKA_RESP_ERR_UNKNOWN
#### RD_KAFKA_RESP_ERR_NO_ERROR
#### RD_KAFKA_RESP_ERR_OFFSET_OUT_OF_RANGE
#### RD_KAFKA_RESP_ERR_INVALID_MSG
#### RD_KAFKA_RESP_ERR_UNKNOWN_TOPIC_OR_PART
#### RD_KAFKA_RESP_ERR_INVALID_MSG_SIZE
#### RD_KAFKA_RESP_ERR_LEADER_NOT_AVAILABLE
#### RD_KAFKA_RESP_ERR_NOT_LEADER_FOR_PARTITION
#### RD_KAFKA_RESP_ERR_REQUEST_TIMED_OUT
#### RD_KAFKA_RESP_ERR_BROKER_NOT_AVAILABLE
#### RD_KAFKA_RESP_ERR_REPLICA_NOT_AVAILABLE
#### RD_KAFKA_RESP_ERR_MSG_SIZE_TOO_LARGE
#### RD_KAFKA_RESP_ERR_STALE_CTRL_EPOCH
#### RD_KAFKA_RESP_ERR_OFFSET_METADATA_TOO_LARGE

#### RD_KAFKA_MSG_PARTITIONER_RANDOM

Random partitioner.

This is the default partitioner.

Returns a random partition between 0 and the number of partitions minus 1.

#### RD_KAFKA_MSG_PARTITIONER_CONSISTENT

Consistent partitioner.

Uses consistent hashing to map identical keys onto identical partitions.

Returns a partition between 0 and number of partitions minus 1 based on the crc value of the key.

## Credits

Documentation copied from [librdkafka.h](https://github.com/edenhill/librdkafka/blob/master/src/rdkafka.h).

Authors: see [contributors](https://github.com/arnaud-lb/php-rdkafka/graphs/contributors).

## License

php-rdkafka is released under the [MIT](https://github.com/arnaud-lb/php-rdkafka/blob/master/LICENSE) license.
