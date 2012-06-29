--TEST--
Herited class is caught (with selector on parent)
--FILE--
<?php 

class A {
	public function test () {
		return "test";
	}
}

class B extends A {
}
aop_add_around("A::test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[test]
