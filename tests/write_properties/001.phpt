--TEST--
Write Property Test
--FILE--
<?php 

class Tracer {
    private $_modified = array ();

    public function touch ($pObject, $pVarName, $pValue) {
        $this->_modified[] = $pVarName;
    }
    public function getModified () {
        return $this->_modified;
    }
}

class A {


}


$tracer = new Tracer ();

aop_add_before_write("A::*", array ($tracer, 'touch'));

$test = new A();
$test->var1 = 'test';
$test->var2 = 'test2';

var_dump($tracer->getModified());

?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "var1"
  [1]=>
  string(4) "var2"
}
