--TEST--
private method and properties in parent class
--FILE--
<?php

class A {
    private $test = "toto";
}

class B extends A {
    private $test = "test";
    public function test () {
        echo $this->test;
    }
    private $container;
    public function write ($test) {
        $this->container = $test;
        echo '['.$this->container.']';
    }
}


class C extends B {

}
aop_add_before ("read ControllerTest::tttt", function () {});

$b = new C ();
$b->test();


aop_add_before("write testsets->testse", function () {


});

$b->write ('test');
?>
--EXPECT--
test[test]
