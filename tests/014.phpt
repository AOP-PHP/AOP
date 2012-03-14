--TEST--
Method is Partial Joker
--FILE--
<?php 

class A {
	public function adminTest () {
		return "test";
	}

	public function adminTest2 () {
		return "test2";
	}

	public function test3 () {
		return "test3";
	}
}

class B extends A {
}
AOP_add("A::admin*", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->adminTest();
echo $test->adminTest2();
echo $test->test3();

?>
--EXPECT--
[test][test2]test3
