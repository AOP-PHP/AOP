--TEST--
Write Property Selector -> ->$ :: ::$
--FILE--
<?php 

class Tracer {
    private $_modified = array ();

    public function touch1 ($pObject) {
        $pVarName = $pObject->getTriggeringPropertyName();
        $this->_modified[] = $pVarName.'1';
    }
    public function touch2 ($pObject) {
        $pVarName = $pObject->getTriggeringPropertyName();
        $this->_modified[] = $pVarName.'2';
    }
    public function touch3 ($pObject) {
        $pVarName = $pObject->getTriggeringPropertyName();
        $this->_modified[] = $pVarName.'3';
    }
    public function touch4 ($pObject) {
        $pVarName = $pObject->getTriggeringPropertyName();
        $this->_modified[] = $pVarName.'4';
    }
    public function getModified () {
        return $this->_modified;
    }
}

class A {


}


$tracer = new Tracer ();
aop_add_before("write A::*", array ($tracer, 'touch1'));
aop_add_before("write A->*", array ($tracer, 'touch2'));
aop_add_before("write A::$*", array ($tracer, 'touch3'));
aop_add_before("write A->$*", array ($tracer, 'touch4'));
$test = new A();
$test->var1 = 'test';
$test->var2 = 'test2';

var_dump($tracer->getModified());

?>
--EXPECT--
array(8) {
  [0]=>
  string(5) "var11"
  [1]=>
  string(5) "var12"
  [2]=>
  string(5) "var13"
  [3]=>
  string(5) "var14"
  [4]=>
  string(5) "var21"
  [5]=>
  string(5) "var22"
  [6]=>
  string(5) "var23"
  [7]=>
  string(5) "var24"
}
