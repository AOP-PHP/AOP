--TEST--
Testing if echo is hooked
--FILE--
<?php
$a=function(AopJoinpoint $j) {die('Had echoed');};
aop_add_after('echo()',$a);
echo('Say...');
--EXPECT--
Say...Had echoed
