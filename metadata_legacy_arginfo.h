/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7bdf537cb18915955d6c3f1d4775dcc9fc43eb4a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_Metadata_getOrigBrokerId, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_Metadata_getOrigBrokerName arginfo_class_RdKafka_Metadata_getOrigBrokerId

#define arginfo_class_RdKafka_Metadata_getBrokers arginfo_class_RdKafka_Metadata_getOrigBrokerId

#define arginfo_class_RdKafka_Metadata_getTopics arginfo_class_RdKafka_Metadata_getOrigBrokerId


ZEND_METHOD(RdKafka_Metadata, getOrigBrokerId);
ZEND_METHOD(RdKafka_Metadata, getOrigBrokerName);
ZEND_METHOD(RdKafka_Metadata, getBrokers);
ZEND_METHOD(RdKafka_Metadata, getTopics);


static const zend_function_entry class_RdKafka_Metadata_methods[] = {
	ZEND_ME(RdKafka_Metadata, getOrigBrokerId, arginfo_class_RdKafka_Metadata_getOrigBrokerId, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata, getOrigBrokerName, arginfo_class_RdKafka_Metadata_getOrigBrokerName, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata, getBrokers, arginfo_class_RdKafka_Metadata_getBrokers, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata, getTopics, arginfo_class_RdKafka_Metadata_getTopics, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_RdKafka_Metadata(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "RdKafka", "Metadata", class_RdKafka_Metadata_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
