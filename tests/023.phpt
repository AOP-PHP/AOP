--TEST--
No autoload for selector
--FILE--
<?php
class B {
public function test () {
}
} 
spl_autoload_register (function ($e) { echo $e."---"; });
AOP_add("A::test", function ($pObj) { });
function test () {
}
$b = new B();

$b->test();
echo "OK";
?>
--EXPECT--
OK
