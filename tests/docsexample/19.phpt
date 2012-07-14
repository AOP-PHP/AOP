--TEST--
An advice that updates the assigned value of a property
--FILE--
<?php
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopJoinPoint $aop_tjp)
    {
       $assigned = & $aop_tjp->getAssignedValue();
       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $developper->preferences = 'Java';

    echo "This developper loves ", $developper->preferences;
?>
--EXPECT--
This developper loves Java and PHP
