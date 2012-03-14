--TEST--
Exception
--FILE--
<?php 

class mytest {
	public function test () {
		throw new Exception ("exception");
	}
}

AOP_add("mytest::test", function ($pObj) {return $pObj->process();});
$test = new mytest();
try {
	echo $test->test();
} catch (Exception $e) {
	echo "catch".$e->getMessage();
}

?>
--EXPECT--
catchexception
