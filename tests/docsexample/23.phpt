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

namespace Test {
 aop_add_before('foo()', function (\AopTriggeredJoinPoint $atjp) { echo $atjp->getTriggeringFunctionName(), "-> foo()\n"; });
 aop_add_before('foo*()', function (\AopTriggeredJoinPoint $atjp) { echo $atjp->getTriggeringFunctionName(), "-> foo*()\n"; });
 aop_add_before('*bar()', function (\AopTriggeredJoinPoint $atjp) { echo $atjp->getTriggeringFunctionName(), " -> *bar()\n"; });
 aop_add_before('**\\foo()', function (\AopTriggeredJoinPoint $atjp) { echo $atjp->getTriggeringFunctionName(), " -> **\\foo()\n";});
 aop_add_before('foo\\bar()', function (\AopTriggeredJoinPoint $atjp) {echo $atjp->getTriggeringFunctionName(), " -> foo\\bar()\n";});
 aop_add_before('foo\\b*()', function (\AopTriggeredJoinPoint $atjp) {echo $atjp->getTriggeringFunctionName(), " -> foo\\b*()\n";});
 aop_add_before('foobar*\\*()', function (\AopTriggeredJoinPoint $atjp) {echo $atjp->getTriggeringFunctionName(), " -> foobar*\\*()\n";});
 aop_add_before('*\\*\\bar()', function (\AopTriggeredJoinPoint $atjp) { echo $atjp->getTriggeringFunctionName(), " -> *\\*\\bar()\n";});

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
foo\foo -> **\foo()
foo\bar -> foo\bar()
foo\bar -> foo\b*()
foo\bigbar -> foo\b*()
foo\barfoobar -> foo\b*()
foo\bar\bar() -> *\*\bar()
foobar\anything -> foobar*\*()
foobarAnything\foo -> **\foo()
foobarAnything\foo -> foobar*\*()
foobar\foo\bar -> *\*\bar()

