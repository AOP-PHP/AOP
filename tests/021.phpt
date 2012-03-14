--TEST--
private data
--FILE--
<?php 

class A {
	private $data="value";
	public function test () {
		return "test".$this->data;
	}
}

AOP_add("A::test", function ($pObj) { return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();

?>
--EXPECT--
[testvalue]
