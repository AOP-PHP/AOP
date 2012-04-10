--TEST--
Function call with a Joker in selector
--FILE--
<?php 

function test () {
	return "intest";
}

AOP_add_around("te*", function ($pObj) {return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
[intest]
