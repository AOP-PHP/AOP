--TEST--
Aop give resources
--FILE--
<?php

$res = aop_add_after('toto()', function () {});
var_dump($res);

$resp = aop_add_after('object::toto', function () {});
var_dump($resp);
?>
--EXPECTF--
resource(%s) of type (AOP Pointcut)
resource(%s) of type (AOP Pointcut) 

