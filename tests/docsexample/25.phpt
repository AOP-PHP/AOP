--TEST--
Test of method syntax for pointcuts
--FILE--
<?php
namespace {
   class Foo {
      public function bar () {}
      public function fooBar () {}
      public function barFoo(){}
      public function callingBarFoo() {
         echo "While in Foo->callingBarFoo {\n";
         $this->barFoo();
         echo "}\n";
      }
      private function privateBarFoo() {}
      public function callingPrivateBarFoo() {
         $this->privateBarFoo();
      }
   }
}
namespace namespaceFoo {
   class Foo {
      public function bar () {}
      public function fooBar () {}
      public function callingFooBar () {
         $this->fooBar();
      }  
      public function callingFoo_FooBar () {
         echo "While in namespaceFoo_callingFoo_FooBar {\n";
         $foo = new \Foo();
         $foo->fooBar();
         echo "}\n";
      }
   }
}

namespace Test {

aop_add_before('Foo->bar()', function (\AopJoinPoint $atjp) {
   echo $atjp->getClassName(), '->', $atjp->getMethodName(), " => Foo->bar()\n";
});
aop_add_before('Foo->*bar()', function (\AopJoinPoint $atjp) {
   echo $atjp->getClassName(), '->', $atjp->getMethodName(), " => Foo->*bar()\n";
});
aop_add_before('foo->bar*()', function (\AopJoinPoint $atjp) {
    echo $atjp->getClassName(), '->', $atjp->getMethodName(), " => foo->bar*()\n";
});
aop_add_before('foo->*bar*()', function (\AopJoinPoint $atjp) {
    echo $atjp->getClassName(), '->', $atjp->getMethodName(), " => foo->*bar*()\n";
});

$foo = new \Foo();
$namespaceFoo_Foo = new \namespaceFoo\foo();

$foo->bar();
$foo->Bar();
$namespaceFoo_Foo->bar();
$foo->fooBar();
$foo->Foobar();
$namespaceFoo_Foo->fooBar();
$namespaceFoo_Foo->fooBar();
$foo->barFoo();
$foo->BArFoO();
$foo->callingBarFoo();
$namespaceFoo_Foo->callingFooBar();
$namespaceFoo_Foo->callingFoo_FooBar();
}
?>
--EXPECT--
Foo->bar => Foo->bar()
Foo->bar => Foo->*bar()
Foo->bar => foo->bar*()
Foo->bar => foo->*bar*()
Foo->bar => Foo->bar()
Foo->bar => Foo->*bar()
Foo->bar => foo->bar*()
Foo->bar => foo->*bar*()
Foo->fooBar => Foo->*bar()
Foo->fooBar => foo->*bar*()
Foo->fooBar => Foo->*bar()
Foo->fooBar => foo->*bar*()
Foo->barFoo => foo->bar*()
Foo->barFoo => foo->*bar*()
Foo->barFoo => foo->bar*()
Foo->barFoo => foo->*bar*()
Foo->callingBarFoo => foo->*bar*()
While in Foo->callingBarFoo {
Foo->barFoo => foo->bar*()
Foo->barFoo => foo->*bar*()
}
While in namespaceFoo_callingFoo_FooBar {
Foo->fooBar => Foo->*bar()
Foo->fooBar => foo->*bar*()
}
