--TEST--
Tests if the directive aop.enable is correctly handled
--FILE--
<?php
    ini_set("aop.enable", "1");
    echo "aop is enabled\n";
    function foo () {
       echo "I'm foo\n";
    }
    $adviceShowFoo = function () {
       echo "After foo\n";
    };
    
    aop_add_after('foo()', $adviceShowFoo);
    foo();

    ini_set('aop.enable', '0');
    echo "aop is now disabled\n";
    foo();
    echo "But you can still register new aspects\n";
    aop_add_after('f*()', $adviceShowFoo);
    foo();
    
    ini_set('aop.enable', '1');
    echo "Aop is now enabled\n";
    foo();
?>
--EXPECT--
aop is enabled
I'm foo
After foo
aop is now disabled
I'm foo
But you can still register new aspects
I'm foo
Aop is now enabled
I'm foo
After foo
After foo
