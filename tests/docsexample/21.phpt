--TEST--
setAssignedValue
--FILE--
<?php
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopJoinPoint $aop_tjp)
    {
       $assigned = $aop_tjp->getAssignedValue();

       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }

       $aop_tjp->setAssignedValue($assigned);
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $loveWhat = 'Python';
    $developper->preferences = $loveWhat;

    echo "This developper loves ", $developper->preferences;
    echo ", Yes, $loveWhat";
?>
--EXPECT--
This developper loves Python and PHP, Yes, Python

