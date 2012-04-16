--TEST--
Method in selector is partial with a Joker
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

AOP_add_around("A::admin*", function ($pObj) {return "[".$pObj->process()."]";});
$test = new A();
echo $test->adminTest();
echo $test->adminTest2();
echo $test->test3();

?>
--EXPECT--
[test][test2]test3
