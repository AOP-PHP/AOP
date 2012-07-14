--TEST--
Docs Test 8
--FILE--
<?php
    class DivideByZeroException extends Exception {}

    class DivideServices
    {
       public function divide ($number, $divideBy)
       {
          if ($divideBy == 0) {
             throw new DivideByZeroException("Cannot divide by zero");
          }
          echo $number / $divideBy;
       }
    }

    //the advice is a static function of a given class
    $advice = function (AopJoinPoint $joinpoint) {
        //do stuff before
        $args = $joinpoint->getArguments();
        echo " {$args[0]} by {$args[1]} equals [";
        try {
            echo $joinpoint->process();//asks for the joinpoint to be processed as normal
        } catch (DivideByZeroException $e) {
           echo "Infinity";
        }
        echo "]";//do stuff after
    };

    aop_add_around('DivideServices::divide()', $advice);

    $services = new DivideServices();
    $services->divide(4, 2);
    $services->divide(4, 0);

?>
--EXPECT--
4 by 2 equals [2] 4 by 0 equals [Infinity]

