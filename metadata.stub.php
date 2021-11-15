<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka;

class Metadata
{
    public function getOrigBrokerId(): int {}

    public function getOrigBrokerName(): string {}

    public function getBrokers(): Metadata\Collection {}

    public function getTopics(): Metadata\Collection {}
}
