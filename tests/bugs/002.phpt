--TEST--
Call tostring is property is object
--FILE--
<?php 

class A {
    public $test = "data";
}
$name = new SimpleXMLElement("<test>test</test>");
$test = new A();
echo $test->$name;
aop_add_before("A::test", function ($pObj) {
    echo "catch";
});
echo $test->$name;
$test->$name = "toto";
?>
--EXPECT--
datacatchdatacatch
