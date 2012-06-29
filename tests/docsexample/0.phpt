--TEST--
Docs Test 0
--FILE--
<?php    class MyServices
    {
       public function doStuff ()
       {
          echo "do my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function () {
        echo "I was called before doing stuff...";
    };

    aop_add_before('MyServices::doStuff()', $advice);

    $services = new MyServices();
    $services->doStuff();

?>
--EXPECT--
I was called before doing stuff...do my best stuff !

