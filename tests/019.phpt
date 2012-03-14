--TEST--
processWithArgs
--FILE--
<?php 

class mytest {
	public function test ($data) {
		var_dump($data);
	}
}

AOP_add("mytest::test",function ($pObj) {echo "OVERLOAD\n";var_dump($pObj->getArgs());$pObj->processWithArgs(array('overload'));});
AOP_add("mytest::test",function ($pObj) {echo "NOOVERLOAD\n";var_dump($pObj->getArgs());$pObj->process();});

$test = new mytest();
$test->test("first");

?>
--EXPECT--
NOOVERLOAD
array(1) {
  [0]=>
  string(5) "first"
}
OVERLOAD
array(1) {
  [0]=>
  string(5) "first"
}
string(8) "overload"

