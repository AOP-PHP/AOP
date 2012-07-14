--TEST--
Docs Test 9
--FILE--
<?php    class MyServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple function
    function adviceUpdatingArguments (AopJoinPoint $object)
    {
       $args = $object->getArguments();
       if ($args[0] === null) {
          $args[0] = 'anyone';
          $object->setArguments($args);
       }
       $object->process();
    }

    aop_add_around('MyServices::doStuff()', 'adviceUpdatingArguments');

    $services = new MyServices();
    $services->doStuff(null);

?>
--EXPECT--
I'll do my best stuff for anyone !

