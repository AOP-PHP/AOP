--TEST--
Testing invalid callback given to aop_add_after()
--FILE--
<?php 

class mytest {
	public function test () {
		return "intest";
	}
}

aop_add_after("mytest->test()", array('Foo', 'bar'));
$test = new mytest();
echo $test->test();

?>
--EXPECTF--
Warning: aop_add_after() expects parameter 2 to be a valid callback, class 'Foo' not found in %s009.php on line 9

Fatal error: aop_add_after() expects a string for the pointcut as a first argument and a callback as a second argument in %s009.php on line 9

