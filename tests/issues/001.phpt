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
Fatal error: You must specify a class and a property name in %stests/issues/001.php on line 6

