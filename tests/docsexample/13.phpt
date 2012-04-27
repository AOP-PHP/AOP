--TEST--
Docs Test 13
--FILE--
<?php    function callMe ($name, & $reference, & $reference2)
    {
       echo "$name, $reference and $reference2. ";
       $name = "M - $name";
       $reference = "M - $reference";
       $reference2 = "M - $reference2";
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       $args = $joinpoint->getArguments();
       $args[0] = "NEW {$args[0]}";
       $args[1] = "NEW {$args[1]}";
       $args[2] = "NEW {$args[2]}";
       $joinpoint->setArguments($args);
    };

    aop_add_before('callMe', $advice);

    $name = "name";
    $reference = "reference";
    $reference2 = "reference2";
    callMe($name, $reference, $reference2);
    echo "After the method execution, value of name is $name, reference is $reference and reference2 is $reference2";

?>
--EXPECT--
NEW name, NEW reference and NEW reference2. After the method execution, value of name is name, reference is M - NEW reference and reference2 is M - NEW reference2


