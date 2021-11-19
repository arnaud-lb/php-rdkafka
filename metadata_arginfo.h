/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 218d2b82c291fccf0934e9488ee6aebae2c032b4 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_getOrigBrokerId, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_getOrigBrokerName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RdKafka_Metadata_getBrokers, 0, 0, RdKafka\\Metadata\\Collection, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_Metadata_getTopics arginfo_class_RdKafka_Metadata_getBrokers


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
