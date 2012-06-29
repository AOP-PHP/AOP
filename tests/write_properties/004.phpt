--TEST--
Read / Write Property with inc or dec
--FILE--
<?php 

class A {
    public $var1=5;

}


aop_add_before("write A::*", function () { echo "WRITE\n";});
aop_add_before("read A::*", function () { echo "READ\n";});

$test = new A();
$test->var1++;
++$test->var1;
$test->var1--;
--$test->var1;

$test->var2++;
++$test->var2;
$test->var2--;
--$test->var2;


?>
--EXPECT--
READ
WRITE
READ
WRITE
READ
WRITE
READ
WRITE
READ
WRITE
READ
WRITE
READ
WRITE
READ
WRITE
