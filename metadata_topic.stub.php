<?php

/**
 * @generate-class-entries
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka\Metadata;

class Topic
{
    /** @tentative-return-type */
    public function getTopic(): string {}

    /** @tentative-return-type */
    public function getErr(): int {}

    /** @tentative-return-type */
    public function getPartitions(): \RdKafka\Metadata\Collection {}
}
