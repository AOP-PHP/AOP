--TEST--
Test the hook on a property read / write
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_before('Hero->name', function (AopTriggeredJoinPoint $tjp) {
   echo "Triggered";
});

$wizard = new Hero();
$wizard->name = $wizard->name;
--EXPECT--
TriggeredTriggered
