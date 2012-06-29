--TEST--
Not Parent class, not caucht
--FILE--
<?php 

class A {
	public function test () {
		return "test";
	}
}

class B {
	public function test () {
		return "test";
	}
}
aop_add_around("A::test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
test
