--TEST--
Two call to getFunctionName doesn't segfault
--FILE--
<?php 

class A {
	private $data="value";
	public function test () {
		return "test".$this->data;
	}
}

AOP_add("A::test", function ($pObj) { 
	return $pObj->getFunctionName().'|'.$pObj->getFunctionName();
});
$test = new A();
echo $test->test();

?>
--EXPECT--
a::test|a::test
