<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */

namespace {
    abstract class RdKafka {
        private function __construct() {}

        public function addBrokers(string $broker_list): int {}

        public function getMetadata(bool $all_topics, ?RdKafka\Topic $only_topic, int $timeout_ms): RdKafka\Metadata {}

        public function getOutQLen(): int {}

        /**
         * @alias RdKafka::getMetadata
         * @deprecated
         */
        public function metadata(bool $all_topics, ?RdKafka\Topic $only_topic, int $timeout_ms): RdKafka\Metadata {}

        /** @deprecated */
        public function setLogLevel(int $level): void {}

        public function newTopic(string $topic_name, ?RdKafka\Conf $topic_conf = null): RdKafka\Topic {}

        /**
         * @alias RdKafka::getOutQLen
         * @deprecated
         */
        public function outqLen(): int {}

        public function poll(int $timeout_ms): int {}

        public function flush(int $timeout_ms): int {}

#ifdef HAS_RD_KAFKA_PURGE
        public function purge(int $purge_flags): int {}
#endif

        /** @deprecated */
        public function setLogger(int $logger): void {}

        public function queryWatermarkOffsets(string $topic, int $partition, int &$low, int &$high, int $timeout_ms): void {}

        public function offsetsForTimes(array $topic_partitions, int $timeout_ms): array {}

        public function pausePartitions(array $topic_partitions): array {}

        public function resumePartitions(array $topic_partitions): array {}
    }
}

namespace RdKafka {
    class Consumer extends \RdKafka {
        public function __construct(?Conf $conf = null) {}

        public function newQueue(): Queue {}
    }

    class Producer extends \RdKafka {
        public function __construct(?Conf $conf = null) {}

#ifdef HAS_RD_KAFKA_TRANSACTIONS
        public function initTransactions(int $timeout_ms): void {}

        public function beginTransaction(): void {}

        public function commitTransaction(int $timeout_ms): void {}

        public function abortTransaction(int $timeout_ms): void {}
#endif
    }
}
