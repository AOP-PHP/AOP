--TEST--
Method getThis can overload public data
--FILE--
<?php 

class A {
	public $data="value";
	public function test () {
		return "test".$this->data;
	}
}

AOP_add("A::test", function ($pObj) {$pObj->getThis()->data="overload"; return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();

?>
--EXPECT--
[testoverload]
