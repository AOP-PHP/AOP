--TEST--
Function call (exact name)
--FILE--
<?php 

function test () {
	return "intest";
}

AOP_add_around("test", function ($pObj) {return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
[intest]
