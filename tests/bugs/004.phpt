--TEST--
aop on private property
--FILE--
<?php


class A {

    static private $_data = null;
    public static function set ($pData) {
        $data = new StdClass();
        $data->toto = 'titi';
        self::$_data = $pData;
    }
    public static function write () {
        echo self::$_data;
    }
}

aop_add_around ('private An::tte', function ($pObj) {
    echo "catch";
    $pObj->process();
});

    
        A::set('test');
        A::write();
?>
--EXPECT--
test
