--TEST--
Inheritance of interfaces are caught (with selector on parent interface)
--FILE--
<?php 
interface IBaseA {}
interface IA extends IBaseA {}
class A implements IA {
	public function test () {
		return "test";
	}
}

class B extends A {
}
aop_add_around("IBaseA::test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[test]
