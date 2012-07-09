--TEST--
Pointcut syntax for methods
--FILE--
<?php
namespace {
   class Foo {
      public function bar () {}
      public function fooBar () {}
   }
}
namespace namespaceFoo {
   class Foo {
      public function bar () {}
      public function fooBar () {}
   }
}

namespace Test {
$advice1 = function () { echo "1"; };
$advice2 = function () { echo "2"; };
$advice3 = function () { echo "3"; };
$advice4 = function () { echo "4"; };

aop_add_before('Foo->bar()', $advice1);
aop_add_before('Foo->*bar()', $advice2);
aop_add_before('Foo->bar*()', $advice3);
$foo = new \Foo();
$namespaceFoo_Foo = new \namespaceFoo\foo();

$foo->bar();
$foo->Bar();
$namespaceFoo_Foo->bar();
$foo->fooBar();
$foo->Foobar();
$namespaceFoo_Foo->fooBar();
$namespaceFoo_Foo->fooBar();
}
?>
--EXPECT--
12312322
