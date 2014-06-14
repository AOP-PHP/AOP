--TEST--
Accessing undefined properties
--FILE--
<?php
class Foo {
   public $bar;
   private $private_bar;
   protected $protected_bar;
}

aop_add_before('Foo->undefinied', function(AopJoinPoint $jp) {
    echo "Accessing ", $jp->getPropertyName(), "\n"; 
});

$foo = new Foo();
if (! $foo->undefinied) {
   echo "after the PHP notice\n";
}

$foo->undefinied = 'new property';
echo $foo->undefinied;

?>
--EXPECTF--
Accessing undefinied

Notice: Undefined property: Foo::$undefinied in %s on line 13
after the PHP notice
Accessing undefinied
Accessing undefinied
new property
