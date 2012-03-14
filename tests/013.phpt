--TEST--
Joker
--FILE--
<?php 

function test () {
	return "intest";
}

AOP_add("te*", function ($pObj) {return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
[intest]
