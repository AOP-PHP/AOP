--TEST--
extract symbol table reset after method call
--FILE--
<?php 
class test {
    public function extract () {
        $test = array ('var1'=>'1','var2'=>'2');
        extract($test);
        var_dump(get_defined_vars());
        $this->internal();
        var_dump(get_defined_vars());
        
    }

    public function internal () {

    }
}


$test = new test();

$test->extract();
?>
--EXPECT--
array(3) {
  ["test"]=>
  array(2) {
    ["var1"]=>
    string(1) "1"
    ["var2"]=>
    string(1) "2"
  }
  ["var1"]=>
  string(1) "1"
  ["var2"]=>
  string(1) "2"
}
array(3) {
  ["test"]=>
  array(2) {
    ["var1"]=>
    string(1) "1"
    ["var2"]=>
    string(1) "2"
  }
  ["var1"]=>
  string(1) "1"
  ["var2"]=>
  string(1) "2"
}
