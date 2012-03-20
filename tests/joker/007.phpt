--TEST--
Joker at the begining
--FILE--
<?php 

class A {
	public function Atest () {
		return "test";
	}
	public function Btest () {
		return "test2";
	}
}

AOP_add("A::*test", function ($pObj) {return "[".$pObj->process()."]";});
$test = new A();
echo $test->Atest();
echo $test->Btest();

?>
--EXPECT--
[test][test2]
