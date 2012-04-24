--TEST--
Internal Return
--FILE--
<?php
aop_add_after('str_replace',function ($pObj) {
    var_dump($pObj->getReturnedValue());
});
$return = str_replace('test','test','test');
str_replace('test1','test1','test1');
?>
--EXPECT--
string(4) "test"
string(5) "test1"

