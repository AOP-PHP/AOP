--TEST--
Just an around method test (version with echo)
--FILE--
<?php 

class mytest {
	public function test () {
		echo "intest";
	}
}

aop_add_around("mytest::test", function ($pObj) {echo "before"; $pObj->process(); echo "after"; });
$test = new mytest();
$test->test();

?>
--EXPECT--
beforeintestafter
