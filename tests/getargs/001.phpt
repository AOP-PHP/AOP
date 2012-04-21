--TEST--
getArguments return caught method call arguments
--FILE--
<?php 

class mytest {
	public function test ($data) {
		echo "intest";
	}
}

aop_add_around("mytest::test", function ($pObj) {var_dump($pObj->getArguments());$pObj->process(); });
$test = new mytest();
$test->test("first");

?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
}
intest
