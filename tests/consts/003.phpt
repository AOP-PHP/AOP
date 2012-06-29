--TEST--
Testing AOP_KIND_AFTER constant
--FILE--
<?php 
class mytest {
	public function test () {
		echo "intest";
	}
}
aop_add_after('mytest::test()', function ($pObj) {echo $pObj->getKindOfAdvice() === AOP_KIND_AFTER ? 'yes' : 'no'; });
$test = new mytest();
$test->test();

?>
--EXPECT--
intestyes
