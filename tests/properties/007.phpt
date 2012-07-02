--TEST--
Test the hook on a property while writing data (write only) after writing its value
--FILE--
<?php
class Hero
{
   public $name = 'Gandalf';
}

aop_add_after('write Hero->name', function (AopTriggeredJoinPoint $tjp) {
   $pObject = $tjp->getTriggeringObject();
   $pPropertyName = $tjp->getTriggeringPropertyName();
   echo "wrote the Hero name of ", $pObject->$pPropertyName;
});

$wizard = new Hero();
$wizard->name = "Gandalf the Grey";
--EXPECT--
wrote the Hero name of Gandalf the Grey
