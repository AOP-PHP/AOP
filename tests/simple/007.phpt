--TEST--
Testing invalid callback given to aop_add_around()
--FILE--
<?php 

class mytest {
	public function test () {
		return "intest";
	}
}

aop_add_around("mytest->test()", array('Foo', 'bar'));
$test = new mytest();
echo $test->test();

?>
--EXPECTF--
Fatal error: Uncaught LogicException: Passed array does not specify an existing static method (class 'Foo' not found) in %s
Stack trace:
#0 %s aop_add_around('mytest->test()', Array)
#1 {main}
  thrown in %s on line 9
