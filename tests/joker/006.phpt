--TEST--
Just a joker and test no bug with recursion
--FILE--
<?php 

class A {
	public function test () {
		return "test";
	}
	public function test2 () {
		echo $this->test();
		return "test2";
	}
}

AOP_add("*", function ($pObj) {echo $pObj->getThis()->test(); return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();
echo $test->test2();

?>
--EXPECT--
test[test]testtest[test][test2]
