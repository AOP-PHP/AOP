--TEST--
Joker at the begining
--FILE--
<?php 

class BA {
	public function Atest () {
		return "test";
	}
	public function Btst () {
		return "test2";
	}
}

aop_add_around("*A::*test()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new BA();
echo $test->Atest();
echo $test->Btst();

?>
--EXPECT--
[test]test2
