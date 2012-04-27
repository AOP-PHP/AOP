--TEST--
Docs Test 11
--FILE--
<?php    function callMe ($name, & $reference)
    {
       echo "$name and $reference. ";
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       $args = $joinpoint->getArguments();
       $args[0] = 'NEW Name';//won't update the original $name parameter as it is a value
       $args[1] = 'UPDATED $reference';//WILL update the original $reference parameter as it is a reference
    };

    aop_add_before('callMe', $advice);

    $name = "name";
    $reference = "reference";
    callMe($name, $reference);
    echo "After the method execution, value of name is $name and value of reference is $reference";

?>
--EXPECT--
name and UPDATED $reference. After the method execution, value of name is name and value of reference is UPDATED $reference

