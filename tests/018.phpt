--TEST--
test getArgs
--FILE--
<?php 

class mytest {
	public function test ($data) {
		echo "intest";
	}
}

AOP_add("mytest::test", function ($pObj) {var_dump($pObj->getArgs());$pObj->process(); });
AOP_add("mytest::test", function ($pObj) {var_dump($pObj->getArgs());$pObj->process(); });
$test = new mytest();
$test->test("first");

?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
}
array(1) {
  [0]=>
  string(5) "first"
}
intest
