--TEST--
Traits test
--SKIPIF--
<?php if (!version_compare(phpversion(), "5.4", '>=')) echo "skip >= PHP 5.4 needed\n"; ?>
--FILE--
<?php 

trait mytrait {
	public function test () {
		echo "intest";
	}
}

class mytest {
 use mytrait;
}

aop_add_around("mytrait::test()", function ($pObj) {echo "before"; $pObj->process(); echo "after"; });
$test = new mytest();
$test->test();

?>
--EXPECT--
beforeintestafter
