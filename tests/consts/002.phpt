--TEST--
Testing AOP_KIND_BEFORE constant
--FILE--
<?php 
class mytest {
	public function test () {
		echo "intest";
	}
}
aop_add_before('mytest::test()', function ($pObj) {echo $pObj->getKindOfAdvice() === AOP_KIND_BEFORE_METHOD ? 'yes' : 'no'; });
$test = new mytest();
$test->test();

?>
--EXPECT--
yesintest
