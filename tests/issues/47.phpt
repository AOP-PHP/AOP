--TEST--
getPropertyValue with private properties
--SKIPIF--
skip "Bad test, to be corrected"
--FILE--
<?php
class Foo {
   private $bar = "orig";
   public function test() {
       $this->bar;
   }
}

aop_add_before('Foo->bar', function(AopJoinPoint $jp) {
    echo $jp->getPropertyValue();
});

$foo = new Foo();
$foo->test();
/*
$test = $foo->bar;
$foo->bar = "test";
$foo->bar = "test2";
*/

?>
--EXPECT--
origorigtest

