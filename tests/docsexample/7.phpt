--TEST--
Docs Test 7
--FILE--
<?php    class MyGoodServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple method of an object
    class Evil
    {
        function advice (AopJoinPoint $object)
        {
           $args = $object->getArguments();
           echo "I'll do the worst stuff I can to {$args[0]} ! ... mouhahahahaha !";
        }
    }

    $evil = new Evil();
    aop_add_around('MyGoodServices::doStuff()', array($evil, 'advice'));

    $services = new MyGoodServices();
    $services->doStuff('you');

?>
--EXPECT--
I'll do the worst stuff I can to you ! ... mouhahahahaha !

