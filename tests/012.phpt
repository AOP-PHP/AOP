--TEST--
function call (exact name)
--FILE--
<?php 

function test () {
	return "intest";
}

AOP_add("test", function ($pObj) {return "[".$pObj->process()."]";});
echo test();

?>
--EXPECT--
[intest]
