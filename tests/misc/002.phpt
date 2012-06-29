--TEST--
Using a variable as a selector and updating its value.
--FILE--
<?php 
class mytest {
	public function test () {
		echo "intest";
	}
}
$selector = "mytest::test()";
aop_add_around($selector, function ($pObj) {echo "before"; $pObj->process(); echo "after"; });
$selector = "foobar::foo";
$test = new mytest();
$test->test();

?>
--EXPECT--
beforeintestafter
