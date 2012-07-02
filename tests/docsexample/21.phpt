--TEST--
An advice that do not update the assigned value of a property if not gotten by property
--FILE--
<?php
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopTriggeredJoinPoint $aop_tjp)
    {
       //we get the assigned variable by reference, it will be updated by the advice
       $assigned = $aop_tjp->getAssignedValue();
       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $loveWhat = 'Python';
    $developper->preferences = $loveWhat;

    echo "This developper loves ", $developper->preferences;
    echo ", Not just $loveWhat";

--EXPECT--
This developper loves Python and PHP, Not just Python
