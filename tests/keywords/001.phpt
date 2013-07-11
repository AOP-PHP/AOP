--TEST--
Testing if include, include_once, require and require_once are caught
--SKIPIF--
skip "Not Implemented Yet"
--FILE--
<?php
aop_add_before('include()', function(AopJoinPoint $ajp){
   $arguments = $ajp->getArguments();
   echo "Include was called with {$arguments[0]}";
});

include("empty.php");


aop_add_before('include_once()', function(AopJoinPoint $ajp){
   $arguments = $ajp->getArguments();
   echo "Include_once was called with {$arguments[0]}";
});

include_once("empty_2.php");

aop_add_before('require()', function(AopJoinPoint $ajp){
   $arguments = $ajp->getArguments();
   echo "Require was called with {$arguments[0]}";
});

require('empty_3.php');

aop_add_before('require_once()', function(AopJoinPoint $ajp){
   $arguments = $ajp->getArguments();
   echo "Require_once was called with {$arguments[0]}";
});

require_once('empty_4.php');

?>
--EXPECT--
Include was called with empty.phpInclude_once was called with empty_2.phpRequire was called with empty_3.phpRequire_once was called with empty_4.php
