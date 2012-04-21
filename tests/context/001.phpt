--TEST--
Verify this context in method caught (private version)
--FILE--
<?php 

class A {
	private $data="value";
	public function test () {
		return "test".$this->data;
	}
}

aop_add_around("A::test", function ($pObj) { return "[".$pObj->process()."]";});
$test = new A();
echo $test->test();

?>
--EXPECT--
[testvalue]
