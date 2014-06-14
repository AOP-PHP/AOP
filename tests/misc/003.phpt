--TEST--
Trying to use an array as a selector (won't work)
--FILE--
<?php 
class mytest {
	public function test () {
		echo "intest";
	}
}
$selector = array();
aop_add_around($selector, function ($pObj) {echo "before"; $pObj->process(); echo "after"; });
$test = new mytest();
$test->test();

?>
--EXPECTF--
Warning: aop_add_around() expects parameter 1 to be string, array given in %s on line 8

Fatal error: aop_add_around() expects a string for the pointcut as a first argument and a callback as a second argument in %s on line 8

