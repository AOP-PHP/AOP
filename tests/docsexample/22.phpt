--TEST--
First doc example in aop_add_after in phpdoc
--FILE--
<?php
$advice = function (AopJoinPoint $jp) {
   echo "The function returned the value ", $jp->getReturnedValue(), "\n";
};

class Calculator
{
    public function __construct($initialValue = 0)
    {
        $this->total = $initialValue;
    }

    public function add($number)
    {
        return $this->total += $number;
    }
}

aop_add_after('Calculator->add()', $advice);

$calculator = new Calculator();
$calculator->add(4);
$calculator->add(2);
$calculator->add(1);
?>
--EXPECT--
The function returned the value 4
The function returned the value 6
The function returned the value 7

