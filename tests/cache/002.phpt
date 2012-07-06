--TEST--
Test if handle number is enought and realloc if problem
--FILE--
<?php 

class mytest {
    public $var1 = null;
}

$ar = array ();
for ($i=0;$i<1500;$i++) {
    $arr[] = new mytest();
}
aop_add_before("mytest::var1", function ($pObj) { echo "first\n"; });
$test = new mytest();
$test->var1 = "test";

?>
--EXPECT--
first
