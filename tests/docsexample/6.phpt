--TEST--
Docs Test 6
--FILE--
<?php    class MyGoodServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a static function of a given class
    class Evil
    {
        public static function advice (AopJoinPoint $object)
        {
           echo "I'll do the worst stuff I can to everyone ... mouhahahahaha !";
        }
    }

    aop_add_around('MyGoodServices::doStuff()', array('evil', 'advice'));

    $services = new MyGoodServices();
    $services->doStuff('you');

?>
--EXPECT--
I'll do the worst stuff I can to everyone ... mouhahahahaha !

