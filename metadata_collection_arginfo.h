/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e58865f3786e7e88b1e453e9e47639eb31180ae6 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_Collection_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_Collection_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_Metadata_Collection_key arginfo_class_RdKafka_Metadata_Collection_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_Collection_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RdKafka_Metadata_Collection_rewind arginfo_class_RdKafka_Metadata_Collection_next

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RdKafka_Metadata_Collection_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(RdKafka_Metadata_Collection, count);
ZEND_METHOD(RdKafka_Metadata_Collection, current);
ZEND_METHOD(RdKafka_Metadata_Collection, key);
ZEND_METHOD(RdKafka_Metadata_Collection, next);
ZEND_METHOD(RdKafka_Metadata_Collection, rewind);
ZEND_METHOD(RdKafka_Metadata_Collection, valid);


static const zend_function_entry class_RdKafka_Metadata_Collection_methods[] = {
	ZEND_ME(RdKafka_Metadata_Collection, count, arginfo_class_RdKafka_Metadata_Collection_count, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Collection, current, arginfo_class_RdKafka_Metadata_Collection_current, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Collection, key, arginfo_class_RdKafka_Metadata_Collection_key, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Collection, next, arginfo_class_RdKafka_Metadata_Collection_next, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Collection, rewind, arginfo_class_RdKafka_Metadata_Collection_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(RdKafka_Metadata_Collection, valid, arginfo_class_RdKafka_Metadata_Collection_valid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
