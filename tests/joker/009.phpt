--TEST--
Joker at the end
--FILE--
<?php 

class BA {
	public function test () {
		return "test";
	}
}

aop_add_around("A*::test", function ($pObj) {return "[".$pObj->process()."]";});
$test = new BA();
echo $test->test();

?>
--EXPECT--
test
