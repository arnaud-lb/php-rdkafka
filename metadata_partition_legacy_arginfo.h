/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f037447a0343c97281f5f1d0578154ebaafcb647 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_Metadata_Partition_getId, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_Metadata_Partition_getErr arginfo_class_RdKafka_Metadata_Partition_getId

#define arginfo_class_RdKafka_Metadata_Partition_getLeader arginfo_class_RdKafka_Metadata_Partition_getId

#define arginfo_class_RdKafka_Metadata_Partition_getReplicas arginfo_class_RdKafka_Metadata_Partition_getId

#define arginfo_class_RdKafka_Metadata_Partition_getIsrs arginfo_class_RdKafka_Metadata_Partition_getId


ZEND_METHOD(RdKafka_Metadata_Partition, getId);
ZEND_METHOD(RdKafka_Metadata_Partition, getErr);
ZEND_METHOD(RdKafka_Metadata_Partition, getLeader);
ZEND_METHOD(RdKafka_Metadata_Partition, getReplicas);
ZEND_METHOD(RdKafka_Metadata_Partition, getIsrs);


static const zend_function_entry class_RdKafka_Metadata_Partition_methods[] = {
	ZEND_ME(RdKafka_Metadata_Partition, getId, arginfo_class_RdKafka_Metadata_Partition_getId, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Partition, getErr, arginfo_class_RdKafka_Metadata_Partition_getErr, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Partition, getLeader, arginfo_class_RdKafka_Metadata_Partition_getLeader, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Partition, getReplicas, arginfo_class_RdKafka_Metadata_Partition_getReplicas, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Partition, getIsrs, arginfo_class_RdKafka_Metadata_Partition_getIsrs, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
