# PHP's AOP in action #

## aop_add_before ##

before kind of advices enables you to

*    launch advices before the execution of a given function, without interrupting anything
*    launch advices before the execution of a given function, and to interrupt its execution while raising an exception
*    launch advices before the execution of a given function, and to update the targeted function's arguments
*    launch advices before reading and / or writing an object's property

### A simple advice execution ###

    [php]
    class MyServices
    {
       public function doStuff ()
       {
          echo "do my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function () {
        echo "I was called before doing stuff...";
    };

    aop_add_before('MyServices->doStuff()', $advice);

    $services = new MyServices();
    $services->doStuff();

will output

    [screen]
    I was called before doing stuff...do my best stuff !

### An advice that can interrupt the execution of a function / method ###

    [php]
    class MyServices
    {
       public function doStuff ()
       {
          echo "do my best stuff !";
       }
    }

    //the advice is a simple function
    function adviceToInterruptDoStuff ()
    {
       if (! isset($_SESSION['user'])) {
          throw new Exception ("I will never do that with someone I don't know");
       }
    }

    aop_add_before('MyServices->doStuff()', 'adviceToInterruptDoStuff');

    $services = new MyServices();
    try {
        $services->doStuff();
    } catch (Exception $e) {
        echo $e->getMessage();
    }

will output

    [screen]
    I will never do that with someone I don't know

### An advice that can update the adviced function's arguments ###

    [php]
    class MyServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple function
    function adviceToUpdateArguments (AopTriggeredJoinpoint $object)
    {
       $args = $object->getArguments();
       if ($args[0] === null) {
          $args[0] = 'anyone';
          $object->setArguments($args);
       }
    }

    aop_add_before('MyServices->doStuff()', 'adviceToUpdateArguments');

    $services = new MyServices();
    $services->doStuff(null);

will output

    [screen]
    I'll do my best stuff for anyone !

### An advice that knows you're dealing with an object's property ###

    [php]
    class Paparazzi
    {
        public function alert ()
        {
            echo "Celebrity will act or say something !";
        }
    }

    class Celebrity
    {
       public $publicStuff = 'public thinking';
       private $secretStuff;
       public function act ()
       {
          $this->secretStuff = 'secret';
       }

       public function say ()
       {
          echo $this->publicStuff;
       }
    }

    //the advice is a simple function
    $paparazzi = new Paparazzi();
    $paparazzi_informer = function () use ($paparazzi)
    {
       $paparazzi->alert();
    };
    //As no parenthesis are given in the selector, the advice deals with properties (read / write)
    aop_add_before('Celebrity->*Stuff', $paparazzi_informer);

    $CynthiaBellulla = new Celebrity();
    $CynthiaBellulla->act();
    echo $CynthiaBellulla->say();

will output

    [screen]
    Celebrity will act or say something !Celebrity will act or say something !public thinking

### An advice that is interested only in properties writing ###

    [php]
    class Paparazzi
    {
        public function alert ()
        {
            echo "Celebrity secretly act on something !";
        }
    }

    class Celebrity
    {
       private $secretStuff;
       public function act ()
       {
          //Reads the property (won't trigger the pointcut as it's a read operation)
          $oldValue = $this->secretStuff;

          //Writing the new value.
          $this->secretStuff = 'secret';
       }
    }

    //the advice is a simple function
    $paparazzi = new Paparazzi();
    $paparazzi_informer = function () use ($paparazzi)
    {
       $paparazzi->alert();
    };
    //will be triggered before writing the property privateStuff
    aop_add_before('write Celebrity->secretStuff', $paparazzi_informer);

    $CynthiaBellulla = new Celebrity();
    $CynthiaBellulla->act();

will output

    [screen]
    Celebrity secretly act on something !

### An advice that knows what happened on the hooked properties ###

If you want to, you can accept in your advice an AopTriggeringJoinPoint object that will gives your advice more informations
on what exactly happened.

    [php]
    class Paparazzi
    {
        public function shoot ($who, $what, $from)
        {
            echo "I'm taking pictures of $who doing some $what (that's supposed to be $from)";
        }
    }

    class Celebrity
    {
       private $secret;

       private $name;

       public function __construct ($name)
       {
           $this->name = $name;
       }

       public function getName ()
       {
           return $this->name;
       }

       public function act ()
       {
          //Reads the property (won't trigger the pointcut as it's a read operation)
          $oldValue = $this->secret;

          //Writing the new value.
          $this->secret = 'shoping at london';
       }
    }

    //the advice is a simple function
    $paparazzi = new Paparazzi();
    $paparazzi_informer = function (AopTriggeredJoinPoint $aop_tjp) use ($paparazzi)
    {
       if ($aop_tjp->getKindOfAdvice() === AOP_KIND_BEFORE_READ_PROPERTY) {
          return;//we don't care if the value is just readed
       } elseif ($aop_tjp->getKindOfAdvice() === AOP_KIND_BEFORE_WRITE_PROPERTY) {
          $paparazzi->shoot($aop_tjp->getTriggeringObject()->getName(),//calls getName on the caught celebrity
                            $aop_tjp->getAssignedValue(),//gets the value that should be assigned to $object->secret
                            $aop_tjp->getTriggeringPropertyName()//the name of the caught property
                            );
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('Celebrity->secret', $paparazzi_informer);

    $CynthiaBellulla = new Celebrity('Cynthia Bellula');
    $CynthiaBellulla->act();

will output

    [screen]
    I'm taking pictures of Cynthia Bellula doing some shoping at london (that's supposed to be secret)

### An advice that updates the assigned value of a property ###

    [php]
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopTriggeredJoinPoint $aop_tjp)
    {
       $assigned = & $aop_tjp->getAssignedValue();
       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $developper->preferences = 'Java';

    echo "This developper loves ", $developper->preferences;

will output

    [screen]
    This developper loves Java and PHP

Of course, if ou were assigning a variable to the property, the variable could be updated by the advice using its reference.

    [php]
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopTriggeredJoinPoint $aop_tjp)
    {
       //we get the assigned variable by reference, it will be updated by the advice
       $assigned = & $aop_tjp->getAssignedValue();
       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $loveWhat = 'Python';
    $developper->preferences = $loveWhat;

    echo "This developper loves ", $developper->preferences;
    echo ", Yes, $loveWhat"

will output

    [screen]
    This developper loves Python and PHP, Yes, Python and PHP

But there is no need to update the variable if you don't want to

    [php]
    class Developper
    {
       public $preferences;
    }

    $spread_the_love = function (AopTriggeredJoinPoint $aop_tjp)
    {
       //we get the assigned variable by reference, it will be updated by the advice
       $assigned = $aop_tjp->getAssignedValue();
       if ($assigned !== 'PHP') {
           $assigned .= ' and PHP';
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('write Developper->preferences', $spread_the_love);

    $developper = new Developper();
    $loveWhat = 'Python';
    $developper->preferences = $loveWhat;

    echo "This developper loves ", $developper->preferences;
    echo ", Not just $loveWhat"

will output

    [screen]
    This developper loves Python and PHP, Not just Python

## aop_add_after ##

after kind of advices enables you to

*    do stuff after the matched joinpoint
*    replace the return of the adviced function
*    launch an exception in case of an incorrect / unwanted return

### doing stuff after the triggered joinpoint ###

    [php]
    class MyServices
    {
       public function doStuff ()
       {
          return "do my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function () {
        echo "Did some stuff !\n";
    };

    aop_add_after('MyServices->doStuff()', $advice);

    $services = new MyServices();
    echo $services->doStuff();

will output

    [screen]
    Did some stuff !
    do my best stuff !

Here you can see that the advice is called right after the execution of the triggered joinpoint (return "do my best stuff !"),
but before anything else can occur (echo $services->doSuff()).

### replacing / updating the return value of a triggered joinpoint ###

    [php]
    class MyServices
    {
       public function doStuff ()
       {
          return "doing my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        $returnValue = $joinpoint->getReturnedValue();
        $returnValue = str_replace('best', 'very best', $returnValue);
        $joinpoint->setReturnedValue($returnValue);
    };

    aop_add_after('MyServices->doStuff()', $advice);

    $services = new MyServices();
    echo $services->doStuff();

will output

    [screen]
    doing my very best stuff !

### launching an exception in case of unwanted / incorrect returned value ###

Here we will ask PHP's AOP extension to raise exception if a call to file_get_contents returns FALSE (error).
This may not be a best practice as it could add overhead to native PHP functions, but such a practice can be usefull
if you're using an old PHP library that is not using exceptions as a mean to raise errors.

    [php]
    //creating the advice as a closure
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        $args = $joinpoint->getArguments();
        if ($joinpoint->getReturnedValue() === false) {
           throw new Exception("Cannot read from file '{$args[0]}'");
        }
    };

    aop_add_after('file_get_contents()', $advice);

    try {
       @file_get_contents('foo file that does not exists');
    } catch (Exception $e) {
       echo $e->getMessage();
    }

will output

    [screen]
    Cannot read from file 'foo file that does not exists'

## aop_add_around ##

around kind of advices enables you to

*    completely replace the matched joinpoint (including raising exceptions)
*    do stuff around (before and / or) after the joinpoint, including catching exceptions
*    replacing arguments of the matching joinpoint (as of the before kind of advices)
*    replacing the return of the matching joinpoint (as of the after kind of advices)
*    and of course a mix of all of the above

### Replace the matching join point ###

#### Without any consideration of the triggered joinpoint ####

    [php]
    class MyGoodServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a static function of a given class
    class Evil
    {
        public static function advice (AopTriggeredJoinpoint $object)
        {
           echo "I'll do the worst stuff I can to everyone ... mouhahahahaha !";
        }
    }

    aop_add_around('MyGoodServices->doStuff()', array('evil', 'advice'));

    $services = new MyGoodServices();
    $services->doStuff('you');

will output

    [screen]
    I'll do the worst stuff I can to everyone ... mouhahahahaha !

#### Taking into account some considerations of the matched joinpoint ####

    [php]
    class MyGoodServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple method of an object
    class Evil
    {
        function advice (AopTriggeredJoinpoint $object)
        {
           $args = $object->getArguments();
           echo "I'll do the worst stuff I can to {$args[0]} ! ... mouhahahahaha !";
        }
    }

    $evil = new Evil();
    aop_add_around('MyGoodServices->doStuff()', array($evil, 'advice'));

    $services = new MyGoodServices();
    $services->doStuff('you');

will output

    [screen]
    I'll do the worst stuff I can to you ! ... mouhahahahaha !

### Around the triggered joinpoint ###

    [php]

    class DivideByZeroException extends Exception {}

    class DivideServices
    {
       public function divide ($number, $divideBy)
       {
          if ($divideBy == 0) {
             throw new DivideByZeroException("Cannot divide by zero");
          }
          echo $number / $divideBy;
       }
    }

    //the advice is a static function of a given class
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        //do stuff before
        $args = $joinpoint->getArguments();
        echo " {$args[0]} by {$args[1]} equals [";
        try {
            echo $joinpoint->process();//asks for the joinpoint to be processed as normal
        } catch (DivideByZeroException $e) {
           echo "Infinity";
        }
        echo "]";//do stuff after
    };

    aop_add_around('DivideServices->divide()', $advice);

    $services = new DivideServices();
    $services->divide(4, 2);
    $services->divide(4, 0);

will output

    [screen]
     4 by 2 equals [2] 4 by 0 equals [Infinity]

### An advice that can update the adviced function's arguments ###

    [php]
    class MyServices
    {
       public function doStuff ($name)
       {
          echo "I'll do my best stuff for $name !";
       }
    }

    //the advice is a simple function
    function adviceUpdatingArguments (AopTriggeredJoinpoint $object)
    {
       $args = $object->getArguments();
       if ($args[0] === null) {
          $args[0] = 'anyone';
          $object->setArguments($args);
       }
       $object->process();
    }

    aop_add_around('MyServices->doStuff()', 'adviceUpdatingArguments');

    $services = new MyServices();
    $services->doStuff(null);

will output

    [screen]
    I'll do my best stuff for anyone !

### replacing / updating the return value of a triggered joinpoint ###

    [php]
    class MyServices
    {
       public function doStuff ()
       {
          return "doing my best stuff !";
       }
    }

    //creating the advice as a closure
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        $joinpoint->process();
        $returnValue = $joinpoint->getReturnedValue();
        $returnValue = str_replace('best', 'very best', $returnValue);
        $joinpoint->setReturnedValue($returnValue);
    };

    aop_add_around('MyServices->doStuff()', $advice);

    $services = new MyServices();
    echo $services->doStuff();

will output

    [screen]
    doing my very best stuff !

## AopTriggeredJoinpoint complete reference ##

An instance of AopTriggeredJoinpoint will always be passed to your advices. This object contains several informations,
such as the pointcut who triggered the joinpoint, the arguments, the returned value (if available), the raised exception
(if available), and will enables you to run the expected method in case you are "around" it.

### getKindOfAdvice ###

This will tell you if your advice is run

*    around - Aop::AROUND
*    before - Aop::BEFORE
*    after  - Aop::AFTER
*    on exception - Aop::EXCEPTION
*    after, even if an exception was triggered (final) - Aop::FINAL

    [php]
    $advice = function (AopTriggeredJoinpoint $joinpoint) {
        switch ($joinpoint->getKindOfAdvice()) {
            Aop::AROUND: //...
            break;

            Aop::BEFORE: //...
            break;

            Aop::AFTER: //...
            break;

            Aop::EXCEPTION: //...
            break;

            Aop::FINAL: //...
            break;
        }
    }

### getArguments ###

getArguments will return the triggering method arguments as an indexed array. The resulting array will give values when
the triggering method expected values, and references where the triggering method expected references.

    [php]
    function callMe ($name, & $reference)
    {
       echo "$name and $reference. ";
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       $args = $joinpoint->getArguments();
       $args[0] = 'NEW Name';//won't update the original $name parameter as it is a value
       $args[1] = 'UPDATED $reference';//WILL update the original $reference parameter as it is a reference
    };

    aop_add_before('callMe()', $advice);

    $name = "name";
    $reference = "reference";
    callMe($name, $reference);
    echo "After the method execution, value of name is $name and value of reference is $reference";

will output

    [screen]
    name and UPDATED $reference. After the method execution, value of name is name and value of reference is UPDATED $reference

### setArguments ###

setArguments enables you to replace all the arguments the triggering method will receive. Beware that if you want to keep
references, you will have to explicitely pass them back to setArguments.

    [php]
    function callMe ($name, & $reference, & $reference2)
    {
       echo "$name, $reference and $reference2. ";
       $name = "M - $name";
       $reference = "M - $reference";
       $reference2 = "M - $reference2";
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       $args = $joinpoint->getArguments();

       $args[0] = "NEW {$args[0]}";
       $args[1] = "NEW {$args[1]}";
       $args[2] = "NEW {$args[2]}";

       $newArgs = array();
       $newArgs[0] = $args[0];
       $newArgs[1] = & $args[1];//the reference is kept
       $newArgs[2] = $args[2];//newArgs carry a copy of $args[2], the advice won't be able to update it's value

       $joinpoint->setArguments($newArgs);
    };

    aop_add_before('callMe()', $advice);

    $name = "name";
    $reference = "reference";
    $reference2 = "reference2";
    callMe($name, $reference, $reference2);
    echo "After the method execution, value of name is $name, reference is $reference and reference2 is $reference2";

will output

    [screen]
    NEW name, NEW reference and NEW reference2. After the method execution, value of name is name, reference is M - NEW reference and reference2 is NEW reference2

As a rule of thumb, if you don't want to mind about references, keep the arguments in the resulting array to update
their values and give the array back back to setArguments.

    [php]
    function callMe ($name, & $reference, & $reference2)
    {
       echo "$name, $reference and $reference2. ";
       $name = "M - $name";
       $reference = "M - $reference";
       $reference2 = "M - $reference2";
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       $args = $joinpoint->getArguments();
       $args[0] = "NEW {$args[0]}";
       $args[1] = "NEW {$args[1]}";
       $args[2] = "NEW {$args[2]}";
       $joinpoint->setArguments($args);
    };

    aop_add_before('callMe()', $advice);

    $name = "name";
    $reference = "reference";
    $reference2 = "reference2";
    callMe($name, $reference, $reference2);
    echo "After the method execution, value of name is $name, reference is $reference and reference2 is $reference2";

will output

    [screen]
    NEW name, NEW reference and NEW reference2. After the method execution, value of name is name, reference is M - NEW reference and reference2 is M - NEW reference2


NOTE : you should only use setArguments while proceeding to advice of kind *before* and *around*, else, it might be
confusing to update values of the arguments *after* the execution of the trigering method.

### getReturnedValue ###

getReturnedValue will give you the returned value of the triggering method. getReturnedValue will only be populated
in advices of the kind "after". In every other kind of advices, getReturnedValue will be null.

If the triggering method returns a reference and you want to update the given reference, you will have to explicitely
asks for the reference while calling getReturnedValue.

    [php]
    class Writer
    {
       protected $text;

       public function & getText ()
       {
          $this->text = "some text";
          return $this->text;
       }

       public function echoText ()
       {
          echo $this->text;
       }
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       //You're asking explicitely for the reference
       $result = & $joinpoint->getReturnedValue();
       //Updating the value of the reference
       $result = "This is the new text";
    };

    aop_add_after("Writer->getText()", $advice);

    $writer = new Writer();
    $text = $writer->getText();
    $writer->echoText();

will output

    [screen]
    "This is the new text"

If you do the same without the use of reference the value of "Writer->foo" won't be updated, e.g. :

    [php]
    class Writer
    {
       protected $text;

       public function & getText ()
       {
          $this->text = "some text";
          return $this->text;
       }

       public function echoText ()
       {
          echo $this->text;
       }
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       //You're NOT asking explicitely for the reference
       $result = $joinpoint->getReturnedValue();
       //The returned value of the trigerring method won't be updated
       $result = "This is the new text";
    };

    aop_add_after("Writer->getText()", $advice);

    $writer = new Writer();
    $text = $writer->getText();
    $writer->echoText();

will output

    [screen]
    some text

NOTE : Of course if the triggering method doesn't return a reference, asking or not for the reference won't make
any difference.

### setReturnedValue ###

setReturnedValue enables you to define the resulting value of the triggering method. This function makes sense for advices
of kind after, around, exception and final.

If you are assigning a returned value to a method that was expected to return a reference, the original reference will
be lost and won't be replaced. To replace the content of an original reference, just proceed as explained in the getReturnedValue
documentation.

    [php]



### process ###

The process method allow you to explicitely launch the triggering method.

The process method will only be available for advices of kind around. Any call to process in advices of other kinds will
 launch an AopException with a message like "Cannot launch the process method in an advice of kind XXX".

### getPointcut ###

getPointcut returns the pointcut (as a string) that triggered the joinpoint.

### getTriggeringObject ###

getTriggeringObject returns the object of the triggered joinppoint. If the joinpoint does not belongs to an object,
getTriggeringObject returns null.

### getTriggeringClassName ###

getTriggeringClassName returns the object's class name of the triggered joinpoint. If the joinpoint does not belongs to
a class, getTriggeringClassName returns null.

If the class is declared in a namespace, getTriggeringClassName indicates the full name of the class (with the namespace).

### getTriggeringMethodName ###

## Pointcuts syntax ##

### Basics ###

Selectors will enables you to describe with a very simple syntax functions and methods that should be considered for
raising the execution of a given advice.

At their simpliest form, selectors will be given the name of the function itself, including its namespace.

eg :
*    functionName will raise advices for every call of the function functionName
*    namespaceOne\namespaceTwo\functionName will raise advices for every call of the function functionName in
the namespace namespaceOne\namespaceTwo, but won't be triggered if you're calling only a method named functionName in an
another namespace.

Of course you can specify a method of an object of a given class name, by separating the class name and the method name
by two colons (::).

eg :
*    MyClass::myMethod will be triggered while calling the method myMethod of any instance of the class MyClass
*    \namespaceOne\namespaceTwo\MyClass::myMethod will be triggered while calling the method myMethod of any instance of
the class MyClass in the namespace \namespaceOne\namespaceTwo.

### public / protected / private ###

There is a specitic keyword you can use to tell AOP to consider only methods that are public, protected or private.

eg :
*    public MyClass::myMethod will be triggered while calling public methods named myMethod.
*    public | protected MyClass::myMethod will be triggered while calling public or protected methods named myMethod.

For those keywords, you can use a negation with the exclamation mark (!)

eg :
*    !public MyClass::* will accept every non public method call of objects of type MyClass in the root namespace


### Wildcards ###

Of course you may not want to list for the AOP extension every functions of every class you're interested in having
 pointcuts for. There are case where you would prefer to tell AOP the format of those elements, and that's why there
  are wildcards.

*    *  will accept any function call in the root namespace
*    admin* will accept any function call in the root namespace wich name starts with admin
*    *admin will accept any function call in the root namespace wich name ends with admin
*    namespaceOne/* will accept any function call in the namespace namespaceOne, but not in subnamespaces of namespaceOne
*    namespaceOne/namespaceTwo/* will accept any function call in the namespace namespaceOne/namespaceTwo only
*    */* will accept any function call in any single level namespace (eg namespaceOne/functionName or namespaceTwo/otherFunctionName
but not namespaceOne/namespaceTwo/functionName)
*    admin*/*/cache* will accept functions called with names names that starts with cache in a namespace called admin something
 with a second level namespace of any name (eg adminStuff/anything/cacheStuff)

Wildcards can also be used to specify class names.

*    *::methodName will accept all methods call named methodName in any object (eg Object1::methodName, Object2::methodName
in the root namespace)
*    *Foo*::admin* will accept methods call that start with admin in classes that contains Foo in their names, in the root
namespace

### Specific keywords ###


### Wildcards ###


*    * match anything inside a name but stops when it encounters a /
*    ** match anything, the scope includes the paths (/)

### Simple selectors examples ###

the syntax to describe selectors is quite easy.

*    'functionName' represent any call of a function called 'functionName' in the root namespace
*    'namespaceName\\functionName' represent any call of a function called 'functionName' in the namespaceName namespace
*    'ClassName::methodName' represent any call of a method called methodName from an instance (or not) of a class ClassName in the root namespace
*    'namespaceName\\ClassName::methodName' represent any call of a method called methodName from an instance (or not) of a class
ClassName located in the namespace namespaceName

You can use both :: and -> as a seperator for classes/method (e.g. Class->method equals Class::method).


### Selectors using wildcards examples ###

*    'startingFunctionName*' represent any call of a function who's name starts with startingFunctionName in the root namespace
*    '*endingFunctionName' represent any call of a function who's name ends with endingFunctionName in the root namespace
*    '*\\functionName' represent any call of a function called functionName in any single level namespace
*    '*\\*\\functionName'  represent any call of a function called functionName in any two level namespace
*    'StartingClassName*::methodName' represent any call of a method called methodName from an instance (or not) of a
class who's name start with StartingClassName in the root namespace
*    '*EndingClassName::methodName' represent any call of a method called methodName from an instance (or not) of a
class who's name end with EndingClassName in the root namespace

### Selectors using super wildcards examples ###
*    '**\\' 
*    **/*::admin* represents every call of a method starting by admin of any class in any namespace
*    **\\* represents every call of any method in any namespace
