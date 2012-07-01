--TEST--
Tests on read property (setReturnedValue)
--FILE--
<?php
    class Foo {
        public $var1='before';
    }

    aop_add_after ('read Foo::var1', function ($pObj) {
        $pObj->setReturnedValue ("after");
    });
    
    $foo = new Foo ();
    echo '['.$foo->var1."]\n";
    var_dump($foo);
?>
--EXPECT--
[after]
object(Foo)#2 (1) {
  ["var1"]=>
  string(6) "before"
}
