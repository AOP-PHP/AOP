--TEST--
Scope
--FILE--
<?php 
aop_add_around('public|protected A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "1".$toReturn."\n";
    return $toReturn;
});

aop_add_around('public|private A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "2".$toReturn."\n";
    return $toReturn;
});

aop_add_around('private|protected A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "3".$toReturn."\n";
    return $toReturn;
});

aop_add_around('private|public A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "4".$toReturn."\n";
    return $toReturn;
});
aop_add_around('protected|private A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "5".$toReturn."\n";
    return $toReturn;
});
aop_add_around('protected|public A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "6".$toReturn."\n";
    return $toReturn;
});
aop_add_around('private|protected|public A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "7".$toReturn."\n";
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
3private
4private
5private
7private
1protected
3protected
5protected
6protected
7protected
1public
2public
4public
6public
7public
