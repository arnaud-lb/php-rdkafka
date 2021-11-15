<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka;

class TopicPartition
{
    public function __construct(string $topic, int $partition, ?int $offset = 0) {}

    public function getTopic(): ?string {}

    public function setTopic(string $topic_name): TopicPartition {}

    public function getPartition(): int {}

    public function setPartition(int $partition): TopicPartition {}

    public function getOffset(): int {}

    public function setOffset(int $offset): TopicPartition {}

    public function getErr(): ?int {}
}
