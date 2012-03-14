--TEST--
Instance Class with parent
--FILE--
<?php 

class A {
	public function test () {
		return "test";
	}
}

class B extends A {
}
AOP_add("B::test", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[test]
