--TEST--
Docs Test 2
--FILE--
<?php    class MyServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple function
    function adviceToUpdateArguments (AopJoinPoint $object)
    {
       $args = $object->getArguments();
       if ($args[0] === null) {
          $args[0] = 'anyone';
          $object->setArguments($args);
       }
    }

    aop_add_before('MyServices::doStuff()', 'adviceToUpdateArguments');

    $services = new MyServices();
    $services->doStuff(null);

?>
--EXPECT--
I'll do my best stuff for anyone !

