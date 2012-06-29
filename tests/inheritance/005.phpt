--TEST--
Herited class is caught (with selector on interface on parent)
--FILE--
<?php 
interface IA {}
class A implements IA {
	public function test () {
		return "test";
	}
}

class B extends A {
}
aop_add_around("IA::test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[test]
