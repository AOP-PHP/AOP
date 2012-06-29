--TEST--
Method in selector is just a Joker
--FILE--
<?php 

class A {
	public function test () {
		return "test";
	}
	public function test2 () {
		return "test2";
	}
}

aop_add_around("A::*()", function ($pObj) {return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();
echo $test->test2();

?>
--EXPECT--
[test][test2]
