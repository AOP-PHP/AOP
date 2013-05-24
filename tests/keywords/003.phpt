--TEST--
Testing if print is hooked
--FILE--
<?php
$a=function(AopJoinpoint $j) {die('Had printed');};
aop_add_after('print()',$a);
print('Hello...');
--EXPECT--
Hello...Had printed
