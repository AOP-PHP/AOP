--TEST--
Scope
--FILE--
<?php 
aop_add_around('public A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "1".$toReturn."\n";
    return $toReturn;
});

aop_add_around('private A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "2".$toReturn."\n";
    return $toReturn;
});

aop_add_around('protected A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "3".$toReturn."\n";
    return $toReturn;
});
class A {
    private function testp () {
        return "private";
    }

    protected function testpr () {
        return "protected";
    }
    public function test () {
        $this->testp();
        $this->testpr();
        return "public";
    }
}

$a = new A();
$a->test();

?>
--EXPECT--
2private
3protected
1public
