<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka;

abstract class Topic
{
    public function getName(): string {}
}

class ConsumerTopic extends Topic
{
    /** @implementation-alias RdKafka::__construct */
    private function __construct() {}

    public function consumeQueueStart(int $partition, int $offset, Queue $queue): void {}

    public function consumeCallback(int $partition, int $timeout_ms, callable $callback): int {}

    public function consumeStart(int $partition, int $offset): void {}

    public function consumeStop(int $partition): void {}

    public function consume(int $partition, int $timeout_ms): ?Message {}

    public function consumeBatch(int $partition, int $timeout_ms, int $batch_size): array {}

    public function offsetStore(int $partition, int $offset): void {}
}

class KafkaConsumerTopic extends Topic
{
    /** @implementation-alias RdKafka::__construct */
    private function __construct() {}

    /** @implementation-alias RdKafka\ConsumerTopic::offsetStore */
    public function offsetStore(int $partition, int $offset): void {}
}

class ProducerTopic extends Topic
{
    /** @implementation-alias RdKafka::__construct */
    private function __construct() {}

    public function produce(int $partition, int $msgflags, ?string $payload = null, ?string $key = null, ?string $msg_opaque = null): void {}

#ifdef HAVE_RD_KAFKA_MESSAGE_HEADERS
    public function producev(int $partition, int $msgflags, ?string $payload = null, ?string $key = null, ?array $headers = null, ?int $timestamp_ms = null, ?string $msg_opaque = null): void {}
#endif
}
