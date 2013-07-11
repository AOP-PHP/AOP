--TEST--
Testing if echo is hooked
--SKIPIF--
skip "Not Implemented Yet"
--FILE--
<?php
$a=function(AopJoinpoint $j) {die('Had echoed');};
aop_add_after('echo()',$a);
echo('Say...');
--EXPECT--
Say...Had echoed
