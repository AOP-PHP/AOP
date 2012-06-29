--TEST--
Two call to getTriggeringMethodName doesn't segfault
--FILE--
<?php 

class A {
	private $data="value";
	public function test () {
		return "test".$this->data;
	}
}

aop_add_around("A::test()", function ($pObj) { 
	echo $pObj->getTriggeringMethodName().'|'.$pObj->getTriggeringMethodName();
});
$test = new A();
$test->test();

?>
--EXPECT--
test|test
