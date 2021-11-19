<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka;

class Metadata
{
    /** @tentative-return-type */
    public function getOrigBrokerId(): int {}

    /** @tentative-return-type */
    public function getOrigBrokerName(): string {}

    /** @tentative-return-type */
    public function getBrokers(): Metadata\Collection {}

    /** @tentative-return-type */
    public function getTopics(): Metadata\Collection {}
}
