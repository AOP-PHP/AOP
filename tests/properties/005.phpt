--TEST--
Test the hook on a property after
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_after('Hero->name', function (AopJoinPoint $tjp) {
   echo " the Grey";
});

$wizard = new Hero();
echo $wizard->name;
?>
--EXPECT--
the GreyGandalf 
