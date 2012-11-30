--TEST--
Testing examples of pointcuts for functions in the phpdoc (pointcut.xml)
--FILE--
<?php
namespace Foo {
   function foo(){}
   function bar(){}
}

namespace Bar {
   function foo(){}
   function bar(){}
}


namespace {
   function foo() {}
   function fooBar() {}
   function someBar(){}
   class Foo {
      public function foo() {
      }
   }
   $showMatch = function (AopJoinPoint $jp) {
      echo "The pointcut [".$jp->getPointCut()."] match with ".$jp->getFunctionName()."\n";
   };

   aop_add_before('foo()', $showMatch);
   aop_add_before('foo*()', $showMatch);
   aop_add_before('*bar()', $showMatch);
   aop_add_before('foo\\bar()', $showMatch);
   aop_add_before('foo\\b*()', $showMatch);
   aop_add_before('f*\\bar()', $showMatch);
   aop_add_before('**\\*()', $showMatch);

   foo();
   fooBar();
   someBar();

   foo\bar();
   foo\foo();

   bar\foo();
   bar\bar();
}
--EXPECT--
The pointcut [foo()] match with foo
The pointcut [foo*()] match with foo
The pointcut [**\*()] match with foo
The pointcut [foo*()] match with fooBar
The pointcut [*bar()] match with fooBar
The pointcut [**\*()] match with fooBar
The pointcut [*bar()] match with someBar
The pointcut [**\*()] match with someBar
The pointcut [foo\bar()] match with Foo\bar
The pointcut [foo\b*()] match with Foo\bar
The pointcut [f*\bar()] match with Foo\bar
The pointcut [**\*()] match with Foo\bar
The pointcut [**\*()] match with Foo\foo
The pointcut [**\*()] match with Bar\foo
The pointcut [**\*()] match with Bar\bar
