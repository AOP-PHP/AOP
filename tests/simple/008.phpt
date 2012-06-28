--TEST--
Testing invalid callback given to aop_add_before()
--FILE--
<?php 

class mytest {
	public function test () {
		return "intest";
	}
}

aop_add_before("mytest->test", array('Foo', 'bar'));
$test = new mytest();
echo $test->test();

?>
--EXPECTF--
Warning: aop_add_before() expects parameter 2 to be a valid callback, class 'Foo' not found in %s008.php on line 9

Fatal error: Bad params in %s008.php on line 9
