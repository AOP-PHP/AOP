--TEST--
Write Property Test Test Recursion Loop
--FILE--
<?php 

class Tracer {
    private $_modified = array ();

    private $_original = array ();

    public function touch ($pObject, $pVarName, $pValue) {
        if (!isset ($this->_original[$pVarName])) {
            $this->_original[$pVarName] = $pObject->$pVarName;
        }
        $this->_modified[$pVarName] = $pValue;
        if ($this->_original[$pVarName]==$this->_modified[$pVarName]) {
            unset ($this->_modified[$pVarName]);
        }
    }

    public function getModified () {
        return $this->_modified;
    }
}

class A {


}


$tracer = new Tracer ();


$test = new A();
$test->var1 = 'test';
$test->var2 = 'test2';

aop_add_write_property("A", "*", array ($tracer, 'touch'));

$test->var1 = 'test_modified';
$test->var2 = 'test_modified';

$test->var1 = 'test';

var_dump($tracer->getModified());

?>
--EXPECT--
array(1) {
  ["var2"]=>
  string(13) "test_modified"
}
