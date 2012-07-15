--TEST--
Test the hook on a property while writing data (write only)
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_before('write Hero->name', function (AopJoinPoint $tjp) {
   echo "I shall pass";
});

$wizard = new Hero();
$wizard->name = "Gandalf";
$temp =  $wizard->name;
?>
--EXPECT--
I shall pass
