--TEST--
add simple
--FILE--
<?php 

class mytest {
	public function test () {
		return "intest";
	}
}

AOP_add("mytest::test", function ($pObj) {return "[".$pObj->process()."]";});
$test = new mytest();
echo $test->test();

?>
--EXPECT--
[intest]
