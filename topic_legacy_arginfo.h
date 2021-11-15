/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4d94e1a5976cdda5c264d27a9ee201ddb789040f */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_Topic_getName, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_ConsumerTopic___construct arginfo_class_RdKafka_Topic_getName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consumeQueueStart, 0, 0, 3)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consumeCallback, 0, 0, 3)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, timeout_ms)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consumeStart, 0, 0, 2)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consumeStop, 0, 0, 1)
	ZEND_ARG_INFO(0, partition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consume, 0, 0, 2)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ConsumerTopic_consumeBatch, 0, 0, 3)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, timeout_ms)
	ZEND_ARG_INFO(0, batch_size)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_ConsumerTopic_offsetStore arginfo_class_RdKafka_ConsumerTopic_consumeStart

#define arginfo_class_RdKafka_KafkaConsumerTopic___construct arginfo_class_RdKafka_Topic_getName

#define arginfo_class_RdKafka_KafkaConsumerTopic_offsetStore arginfo_class_RdKafka_ConsumerTopic_consumeStart

#define arginfo_class_RdKafka_ProducerTopic___construct arginfo_class_RdKafka_Topic_getName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ProducerTopic_produce, 0, 0, 2)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, msgflags)
	ZEND_ARG_INFO(0, payload)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, msg_opaque)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_ProducerTopic_producev, 0, 0, 2)
	ZEND_ARG_INFO(0, partition)
	ZEND_ARG_INFO(0, msgflags)
	ZEND_ARG_INFO(0, payload)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, headers)
	ZEND_ARG_INFO(0, timestamp_ms)
	ZEND_ARG_INFO(0, msg_opaque)
ZEND_END_ARG_INFO()


ZEND_METHOD(RdKafka_Topic, getName);
ZEND_METHOD(RdKafka, __construct);
ZEND_METHOD(RdKafka_ConsumerTopic, consumeQueueStart);
ZEND_METHOD(RdKafka_ConsumerTopic, consumeCallback);
ZEND_METHOD(RdKafka_ConsumerTopic, consumeStart);
ZEND_METHOD(RdKafka_ConsumerTopic, consumeStop);
ZEND_METHOD(RdKafka_ConsumerTopic, consume);
ZEND_METHOD(RdKafka_ConsumerTopic, consumeBatch);
ZEND_METHOD(RdKafka_ConsumerTopic, offsetStore);
ZEND_METHOD(RdKafka_ProducerTopic, produce);
ZEND_METHOD(RdKafka_ProducerTopic, producev);


static const zend_function_entry class_RdKafka_Topic_methods[] = {
	ZEND_ME(RdKafka_Topic, getName, arginfo_class_RdKafka_Topic_getName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RdKafka_ConsumerTopic_methods[] = {
	ZEND_MALIAS(RdKafka, __construct, __construct, arginfo_class_RdKafka_ConsumerTopic___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(RdKafka_ConsumerTopic, consumeQueueStart, arginfo_class_RdKafka_ConsumerTopic_consumeQueueStart, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, consumeCallback, arginfo_class_RdKafka_ConsumerTopic_consumeCallback, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, consumeStart, arginfo_class_RdKafka_ConsumerTopic_consumeStart, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, consumeStop, arginfo_class_RdKafka_ConsumerTopic_consumeStop, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, consume, arginfo_class_RdKafka_ConsumerTopic_consume, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, consumeBatch, arginfo_class_RdKafka_ConsumerTopic_consumeBatch, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ConsumerTopic, offsetStore, arginfo_class_RdKafka_ConsumerTopic_offsetStore, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RdKafka_KafkaConsumerTopic_methods[] = {
	ZEND_MALIAS(RdKafka, __construct, __construct, arginfo_class_RdKafka_KafkaConsumerTopic___construct, ZEND_ACC_PRIVATE)
	ZEND_MALIAS(RdKafka_ConsumerTopic, offsetStore, offsetStore, arginfo_class_RdKafka_KafkaConsumerTopic_offsetStore, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RdKafka_ProducerTopic_methods[] = {
	ZEND_MALIAS(RdKafka, __construct, __construct, arginfo_class_RdKafka_ProducerTopic___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(RdKafka_ProducerTopic, produce, arginfo_class_RdKafka_ProducerTopic_produce, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_ProducerTopic, producev, arginfo_class_RdKafka_ProducerTopic_producev, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
