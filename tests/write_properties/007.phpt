--TEST--
Write General API
--FILE--
<?php

class Foo {
    public $var1 = 'before';
    public $var2 = 'before';
    public $var3 = 'before';
}


aop_add_before('write Foo::var1', function ($pObj) {
    $pObject = $pObj->getTriggeringObject();
    $pProperty = $pObj->getTriggeringPropertyName ();
    echo "B ".$pObject->$pProperty."\n";
});

aop_add_after('write Foo::var1', function ($pObj) {
    $pObject = $pObj->getTriggeringObject();
    $pProperty = $pObj->getTriggeringPropertyName ();
    echo "A ".$pObject->$pProperty."\n";
});


aop_add_around ('write Foo::var2', function ($pObj) {
    $pObject = $pObj->getTriggeringObject();
    $pProperty = $pObj->getTriggeringPropertyName ();
    echo "B ".$pObject->$pProperty."\n";
    $pObj->process();
    echo "A ".$pObject->$pProperty."\n";
    
});

aop_add_around ('write Foo::var3', function ($pObj) {}); 

$foo = new Foo ();
$foo->var1="after";
$foo->var2="after";
$foo->var3="after";
var_dump ($foo->var3);
--EXPECT--
B before
A after
B before
A after
string(6) "before"
