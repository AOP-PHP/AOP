--TEST--
Method getTriggeringObject can overload public data
--FILE--
<?php 

class A {
	public $data="value";
	public function test () {
		return "test".$this->data;
	}
}

aop_add_around("A::test()", function ($pObj) {$pObj->getObject()->data="overload"; return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();

?>
--EXPECT--
[testoverload]
