--TEST--
No segfault while giving an empty closure
--FILE--
<?php
function foo () {
echo "function foo()";
}

aop_add_before ('*', function(){});

foo();
echo "end";
?>
--EXPECT--
function foo()end
