--TEST--
Pointcut syntax for functions
--FILE--
<?php
namespace {
 function foo() {}
 function fooo() {}
 function bar() {}
 function foobar() {}
 function foooBar(){}
 function someBar(){}

 class Object {
    public function foo () {
    }
 }
}

namespace Foo {
 function foo(){}
 function bar(){}
 function bigbar(){}
 function barfoobar(){}
}

namespace foo\bar {
 function bar(){}
}

namespace bar {
 function bar(){}
 function foo(){}
}

namespace foobar {
 function foo(){}
 function bar(){}
 function anything(){}
}

namespace foobarAnything {
 function foo(){}
 function bar(){}
 function anything(){}
}

namespace foobar\foo {
 function foo(){}
 function bar(){}
 function foobar(){}
}

namespace some\other {
 function bar(){}
}

namespace some\longer\foo {
 function bar(){}
}

namespace startWithFoo {
 function bar(){}
 function foo(){}
 function bar2(){}
}

namespace startWithFooSomething\anything\anything_else {
 function bar(){}
 function foo(){}
}

namespace Test {
 aop_add_before('foo()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> foo()\n"; });
 aop_add_before('foo*()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> foo*()\n"; });
 aop_add_before('*bar()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> *bar()\n"; });
 aop_add_before('**\\foo()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> **\\foo()\n";});
 aop_add_before('foo\\bar()', function (\AopJoinPoint $atjp) {echo $atjp->getFunctionName(), " -> foo\\bar()\n";});
 aop_add_before('foo\\b*()', function (\AopJoinPoint $atjp) {echo $atjp->getFunctionName(), " -> foo\\b*()\n";});
 aop_add_before('foobar*\\*()', function (\AopJoinPoint $atjp) {echo $atjp->getFunctionName(), " -> foobar*\\*()\n";});
 aop_add_before('*\\*\\bar()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> *\\*\\bar()\n";});
 aop_add_before('StartWithFoo*\\**\\bar()', function (\AopJoinPoint $atjp) { echo $atjp->getFunctionName(), " -> StartWithFoo*\\**\\bar()\n";});

 $object = new \Object();
 \foo();
 \Foo();
 \fOo();
 $object->foo();
 \fooo();
 \fooBar();
 \bar();
 \foooBar();
 \someBar();
 \bar\foo();
 \foo\foo();
 \foo\bar();
 \bar\bar();
 \foo\bigbar();
 \foo\barFooBar();
 \foo\bar\bar();
 \foobar\anything();
 \foobarAnything\foo();
 \foobar\foo\bar();
 \foobar\foo\foobar();
 \some\longer\foo\bar();
 \startWithFoo\bar();
 \StartWithFoo\foo();
 \StartWITHFoo\bar2();
 \startWithFooSomething\anything\anything_else\foo();
 \startWithFooSomething\anything\anything_else\bar();
}
?>
--EXPECT--
foo -> foo()
foo -> foo*()
foo -> **\foo()
foo -> foo()
foo -> foo*()
foo -> **\foo()
foo -> foo()
foo -> foo*()
foo -> **\foo()
fooo -> foo*()
foobar -> foo*()
foobar -> *bar()
bar -> *bar()
foooBar -> foo*()
foooBar -> *bar()
someBar -> *bar()
bar\foo -> **\foo()
Foo\foo -> **\foo()
Foo\bar -> foo\bar()
Foo\bar -> foo\b*()
Foo\bigbar -> foo\b*()
Foo\barfoobar -> foo\b*()
foo\bar\bar -> *\*\bar()
foobar\anything -> foobar*\*()
foobarAnything\foo -> **\foo()
foobarAnything\foo -> foobar*\*()
foobar\foo\bar -> *\*\bar()
startWithFoo\bar -> StartWithFoo*\**\bar()
startWithFoo\foo -> **\foo()
startWithFooSomething\anything\anything_else\foo -> **\foo()
startWithFooSomething\anything\anything_else\bar -> StartWithFoo*\**\bar()
