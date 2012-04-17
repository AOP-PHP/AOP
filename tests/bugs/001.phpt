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

AOP_add_around("A::test", function ($pObj) { 
	return $pObj->getTriggeringMethodName().'|'.$pObj->getTriggeringMethodName();
});
$test = new A();
echo $test->test();

?>
--EXPECT--
test|test
