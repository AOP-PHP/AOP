--TEST--
Write Property Test
--FILE--
<?php 

class Tracer {
    private $_modified = array ();

    public function touch ($pObject) {
        $pVarName = $pObject->getPropertyName();
        $this->_modified[] = $pVarName;
    }
    public function getModified () {
        return $this->_modified;
    }
}

class A {


}


$tracer = new Tracer ();

aop_add_before("write A::*", array ($tracer, 'touch'));

$test = new A();
$test->var1 = 'test';
$test->Var2 = 'test2';

var_dump($tracer->getModified());

?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "var1"
  [1]=>
  string(4) "Var2"
}
