--TEST--
Herited class is caught (with selector on interface on child)
--FILE--
<?php 
interface IA {}
interface IB {}
interface IC {}
interface ID {}
class A  implements IB,IC {
	public function test () {
		return "test";
	}
}

class B extends A implements IA,ID {
}
aop_add_around("IA::test()", function ($pObj) {return "[".$pObj->process()."]";});
aop_add_around("IB::test()", function ($pObj) {return "[".$pObj->process()."]";});
aop_add_around("IC::test()", function ($pObj) {return "[".$pObj->process()."]";});
aop_add_around("ID::test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[[[[test]]]]
