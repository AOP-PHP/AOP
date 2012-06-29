--TEST--
Docs Test 5
--FILE--
<?php    //creating the advice as a closure
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        $args = $joinpoint->getArguments();
        if ($joinpoint->getReturnedValue() === false) {
           throw new Exception("Cannot read from file '{$args[0]}'");
        }
    };

    aop_add_after('file_get_contents()', $advice);

    try {
       @file_get_contents('foo file that does not exists');
    } catch (Exception $e) {
       echo $e->getMessage();
    }

?>
--EXPECT--
Cannot read from file 'foo file that does not exists'

