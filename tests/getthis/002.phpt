--TEST--
In function context getThis return null
--FILE--
<?php 

function test () {
	return "test";
}

AOP_add("test", function ($pObj) {
	if ($pObj->getThis()!=null) {
		echo "error";
	} else {
		echo "OK";
	}
	return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
OK[test]
