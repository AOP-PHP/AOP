--TEST--
Verify this context in method caught (public version)
--FILE--
<?php 

class mytest {
	public $data = "";
	public function test () {
		return "test-".$this->data;
	}
}

aop_add_around("mytest::test", function ($pObj) {return "around".$pObj->process();});
$test = new mytest();
$test->data="prop";
echo $test->test();

?>
--EXPECT--
aroundtest-prop
