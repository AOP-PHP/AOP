--TEST--
Docs Test 4
--FILE--
<?php    class MyServices
    {
       public function doStuff ()
       {
          return "doing my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        $returnValue = $joinpoint->getReturnedValue();
        $returnValue = str_replace('best', 'very best', $returnValue);
        $joinpoint->setReturnedValue($returnValue);
    };

    aop_add_after('MyServices::doStuff()', $advice);

    $services = new MyServices();
    echo $services->doStuff();

?>
--EXPECT--
doing my very best stuff !

