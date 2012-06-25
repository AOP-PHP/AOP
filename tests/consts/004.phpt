--TEST--
Testing that AOP_* consts are different
--FILE--
<?php 
echo AOP_KIND_BEFORE !== AOP_KIND_AFTER ? 'y' : 'n';
echo AOP_KIND_BEFORE !== AOP_KIND_AROUND ? 'y' : 'n';
echo AOP_KIND_AROUND !== AOP_KIND_AFTER ? 'y' : 'n';

?>
--EXPECT--
yyy
