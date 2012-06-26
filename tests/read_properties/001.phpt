--TEST--
Write Property Test
--FILE--
<?php 

class Tracer {
    private $_readed = array ();

    public function touch ($pObject, $pVarName) {
        $this->_readed[] = $pVarName;
    }
    public function getReaded () {
        return $this->_readed;
    }
}

class A {
public $var1 = 'tt';
public $var2 = 'oo';

}


$tracer = new Tracer ();

aop_add_read_property("A", "*", array ($tracer, 'touch'));

$test = new A();
$test->var1 = 'test';
$test->var2 = 'test2';
$var1 = $test->var1;
$var2 = $test->var2;

var_dump($tracer->getReaded());

?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "var1"
  [1]=>
  string(4) "var2"
}
