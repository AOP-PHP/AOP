--TEST--
Test if cache is refresh
--FILE--
<?php 

class mytest {
    public $var1 = null;
}

aop_add_before("mytest::var1", function ($pObj) { echo "first\n"; });
$test = new mytest();
$test->var1 = "test";

aop_add_before("mytest::var1", function ($pObj) { echo "second\n"; });
$test->var1 = "test";
?>
--EXPECT--
first
first
second
