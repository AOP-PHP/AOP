--TEST--
aop on private property
--FILE--
<?php


class A {

    private $_data = null;
    public function set ($pData) {
        $this->_data = $pData;
    }
    public function write () {
        echo $this->_data;
    }
}

aop_add_around ('A::*', function ($pObj) {
    echo "catch";
    $pObj->process();
});

$a = new A ();

$a->set ('test');
$a->write();

?>
--EXPECT--
catchcatchtest
