--TEST--
Test the hook on a property
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_before('Hero->name', function (AopJoinPoint $tjp) {
   echo "Sir ";
});

$wizard = new Hero();
echo $wizard->name;
?>
--EXPECT--
Sir Gandalf
