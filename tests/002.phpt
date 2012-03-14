--TEST--
add simple
--FILE--
<?php 

class mytest {
	public function test () {
		echo "intest";
	}
}

AOP_add("mytest::test", function ($pObj) {echo "before"; $pObj->process(); echo "after"; });
$test = new mytest();
$test->test();

?>
--EXPECT--
beforeintestafter
