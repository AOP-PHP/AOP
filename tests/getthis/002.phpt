--TEST--
In function context getTriggeringObject return null
--FILE--
<?php 

function test () {
	return "test";
}

aop_add_around("test", function ($pObj) {
	if ($pObj->getTriggeringObject()!=null) {
		echo "error";
	} else {
		echo "OK";
	}
	return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
OK[test]
