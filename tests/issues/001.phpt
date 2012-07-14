--TEST--
No segfault with selector just *
--FILE--
<?php
function foo () {
echo "function foo()";
}

aop_add_before ('*', function(){});

foo();
echo "end";
?>
--EXPECTF--
Fatal error: The given pointcut is invalid. You must specify a function call, a method call or a property operation %s

