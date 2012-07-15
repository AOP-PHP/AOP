--TEST--
Test the hook on a property (write only)
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_before('write Hero->name', function (AopJoinPoint $tjp) {
   echo "Sir ";
});

$wizard = new Hero();
echo $wizard->name;
?>
--EXPECT--
Gandalf
