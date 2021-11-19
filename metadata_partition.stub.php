<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace RdKafka\Metadata;

class Partition
{
    /** @tentative-return-type */
    public function getId(): int {}

    /** @tentative-return-type */
    public function getErr(): int {}

    /** @tentative-return-type */
    public function getLeader(): int {}

    /** @tentative-return-type */
    public function getReplicas(): \RdKafka\Metadata\Collection {}

    /** @tentative-return-type */
    public function getIsrs(): \RdKafka\Metadata\Collection {}
}
