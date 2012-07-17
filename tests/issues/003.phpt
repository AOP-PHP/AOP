--TEST--
Validate that aop does not disturb create_function
--FILE--
<?php
$newfunc = create_function('$a,$b', 'return "ln($a) + ln($b) = " . log($a * $b);');
echo $newfunc(2, 2);
?>
--EXPECT--
ln(2) + ln(2) = 1.3862943611199
