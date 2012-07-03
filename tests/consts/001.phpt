--TEST--
Testing AOP_KIND_AROUND constant
--FILE--
<?php 
class mytest {
	public function test () {
		echo "intest";
	}
}
aop_add_around('mytest::test()', function ($pObj) {echo $pObj->getKindOfAdvice() === AOP_KIND_AROUND_METHOD ? 'yes' : 'no'; });
$test = new mytest();
$test->test();

?>
--EXPECT--
yes
