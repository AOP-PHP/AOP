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
Fatal error: Uncaught TypeError: aop_add_around() expects parameter 1 to be string, array given in %s
Stack trace:
#0 %s aop_add_around(Array, Object(Closure))
#1 {main}
  thrown in %s on line 8
