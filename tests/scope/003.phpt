--TEST--
Scope with static
--FILE--
<?php 
aop_add_around('public static A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "1".$toReturn."\n";
    return $toReturn;
});

aop_add_around('public !static A::test*',
function ($pObj) {
    $toReturn = $pObj->process();
    echo "2".$toReturn."\n";
    return $toReturn;
});

aop_add_around('public A::test*',
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
    public static function testst () {
        self::teststp();
        self::teststpr();
        return "staticpublic";
    }
    protected static function teststpr () {
        return "staticprotected";
    }
    private static function teststp () {
        return "staticprivate";
    }
}

$a = new A();
$a->test();
A::testst();

?>
--EXPECT--
3public
2public
3staticpublic
1staticpublic
