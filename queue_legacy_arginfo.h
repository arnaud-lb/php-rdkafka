/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8a39c31916ef5bb148a192245a3f0c7c6ce2dfc3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_Queue___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RdKafka_Queue_consume, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()


ZEND_METHOD(RdKafka, __construct);
ZEND_METHOD(RdKafka_Queue, consume);


static const zend_function_entry class_RdKafka_Queue_methods[] = {
	ZEND_MALIAS(RdKafka, __construct, __construct, arginfo_class_RdKafka_Queue___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(RdKafka_Queue, consume, arginfo_class_RdKafka_Queue_consume, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
