/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5f1c642e1e35859ace5c1d4edb5a0554f5e2f0a0 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_TopicPartition___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, topic, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, partition, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_TopicPartition_getTopic, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_RdKafka_TopicPartition_setTopic, 0, 1, RdKafka\\TopicPartition, 0)
	ZEND_ARG_TYPE_INFO(0, topic_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_TopicPartition_getPartition, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_RdKafka_TopicPartition_setPartition, 0, 1, RdKafka\\TopicPartition, 0)
	ZEND_ARG_TYPE_INFO(0, partition, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_TopicPartition_getOffset arginfo_class_RdKafka_TopicPartition_getPartition

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_RdKafka_TopicPartition_setOffset, 0, 1, RdKafka\\TopicPartition, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_TopicPartition_getErr, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()


ZEND_METHOD(RdKafka_TopicPartition, __construct);
ZEND_METHOD(RdKafka_TopicPartition, getTopic);
ZEND_METHOD(RdKafka_TopicPartition, setTopic);
ZEND_METHOD(RdKafka_TopicPartition, getPartition);
ZEND_METHOD(RdKafka_TopicPartition, setPartition);
ZEND_METHOD(RdKafka_TopicPartition, getOffset);
ZEND_METHOD(RdKafka_TopicPartition, setOffset);
ZEND_METHOD(RdKafka_TopicPartition, getErr);


static const zend_function_entry class_RdKafka_TopicPartition_methods[] = {
	ZEND_ME(RdKafka_TopicPartition, __construct, arginfo_class_RdKafka_TopicPartition___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, getTopic, arginfo_class_RdKafka_TopicPartition_getTopic, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, setTopic, arginfo_class_RdKafka_TopicPartition_setTopic, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, getPartition, arginfo_class_RdKafka_TopicPartition_getPartition, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, setPartition, arginfo_class_RdKafka_TopicPartition_setPartition, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, getOffset, arginfo_class_RdKafka_TopicPartition_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, setOffset, arginfo_class_RdKafka_TopicPartition_setOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_TopicPartition, getErr, arginfo_class_RdKafka_TopicPartition_getErr, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
