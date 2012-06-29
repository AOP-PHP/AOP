--TEST--
Docs Test 3
--FILE--
<?php    class MyServices
    {
       public function doStuff ()
       {
          return "do my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function () {
        echo "Did some stuff !\n";
    };

    aop_add_after('MyServices::doStuff()', $advice);

    $services = new MyServices();
    echo $services->doStuff();

?>
--EXPECT--
Did some stuff !
do my best stuff !

