--TEST--
Issue 34
--FILE--
<?php
function function1() {
echo "function1 ";
}

function function2() {
echo "function2 ";
}

function advice1() {
echo "advice1 ";
// the crash happens here, when advice1() is called by the AOP extension
aop_add_before("function2()", "advice2");
echo "done ";
}

function advice2() {
echo "advice2 ";
echo "done ";
}

aop_add_before("function1()", "advice1");
function1();
function2();
?>
--EXPECT--
advice1 done function1 advice2 done function2
