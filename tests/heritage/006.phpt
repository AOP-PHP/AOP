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
AOP_add("IA::test", function ($pObj) {return "[".$pObj->process()."]";});
AOP_add("IB::test", function ($pObj) {return "[".$pObj->process()."]";});
AOP_add("IC::test", function ($pObj) {return "[".$pObj->process()."]";});
AOP_add("ID::test", function ($pObj) {return "[".$pObj->process()."]";});
$test = new B();
echo $test->test();

?>
--EXPECT--
[[[[test]]]]
