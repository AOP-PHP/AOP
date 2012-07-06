--TEST--
Test if handle number is enought and realloc if problem
--FILE--
<?php 

class mytest {
    public $var1 = null;
}

class truc {
    public $var1 = null;
}

aop_add_before("mytest::var1", function ($pObj) { echo "first\n"; });
$test = new mytest();
$test->var1 = "test";
unset ($test);
$test = new truc();
$test->var1 = "test";

?>
--EXPECT--
first
