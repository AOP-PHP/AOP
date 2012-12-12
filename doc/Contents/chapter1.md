# Introduction #

AOP is a PECL extension that enables you to use Aspect Oriented Programming in PHP, without the need
to compile or proceed to any other intermediate step before publishing your code.

The AOP extension is designed to be the easiest way you can think of for integrating AOP to PHP.

AOP aims to allow separation of cross-cutting concerns (cache, log, security, transactions, ...)

## PHP's AOP extension history ##

The AOP extension is a project which started a while ago even if its development is quite very new (early 2012). It was
first expected to be a fully PHP developed library, as part of a dependency injection framework. The Aspect Oriented
Programming implementation would have taken the form of auto generated proxies.

That was before Julien Salleyron, the lead developer of the project, wanted to take it to the next level while writing
the AOP core features as a PHP's extension.

Gérald Croës also belongs to the initial team, mainly in charge of the documentation and discussions around
the extension's API.

## Installation ##

Download the AOP from github, compile and add the extension to your php.ini

    [bash]
    #Clone the repository on your computer
    git clone https://github.com/AOP-PHP/AOP
    cd AOP
    #prepare the package, you will need to have development tools for php
    phpize
    #compile the package
    ./configure
    make
    #before the installation, check that it works properly
    make test
    #install
    make install

Now you can add the following line to your php.ini to enables AOP

    [ini]
    extension=aop.so

## What is AOP ? Basic tutorial ##

Let's assume the following class

    [php]
    class MyServices
    {
       public function doAdminStuff1 ()
       {
          //some stuff only the admin should do
          echo "Calling doAdminStuff1";
       }

       public function doAdminStuff2 ()
       {
          //some stuff only the admin should do
          echo "Calling doAdminStuff2";
       }
    }

Now you want your code to be safe, you don't want non admin users to be able to call doAdminMethods.

What are your solutions ?

*    Add some code to check the credentials "IN" you MyServices class. The drawback is that it will pollute your
code, and your core service will be less readable.
*    Let the clients have the responsibility to check the credentials when required. The drawbacks are that you will
duplicate lots of code client side if you have to call the service from multiple places
*    Add some kind of credential proxy that will check the credentials before calling the actual service. The drawbacks
are that you will have to write some extra code, adding another class on the top of your services.

Moreover, these solutions tend to increase in complexity while you are adding more cross-cutting concerns like
caching or logging.

That's where AOP comes into action as you will be able to tell PHP to do some extra actions while calling your
MyServices's admin methods.

So let's first write the rule needed to check if we can or cannot access the admin services.

    [php]
    function adviceForDoAdmin ()
    {
       if ((! isset($_SESSION['user_type'])) || ($_SESSION['user_type'] !== 'admin')) {
          throw new Exception('Sorry, you should be an admin to do this');
       }
    }

Dead simple : we check the current PHP session to see if there is something telling us the current user is an admin (of
course we do realize that you may have more complex routines to do that, but we'll keep this for the example).

Now, let's use AOP to tell PHP to execute this method "before" any execution of admin methods.

    [php]
    aop_add_before('MyServices->doAdmin*()', 'adviceForDoAdmin');

Now, each time you'll invoke a method of an object of the class MyServices, starting with doAdmin, AOP will launch the function
basicAdminChecker *before* the called method.

That's it. Simple ain't it ?

Now le's try the examples :

    [php]
    //session is started and we added the above examples to configure MyServices & basicAdminChecker

    $services = new MyServices();
    try {
       $services->doAdminStuff1();//will raise an exception as nothing in the current session tells us we are an admin
    } catch (Exception $e) {
       echo "You cannot access the service, you're not an admin";
    }

    $_SESSION['user_type'] = 'admin';//again, this is ugly for the sake of the example

    try {
       $service->doAdminStuff1();
       $service->doAdminStuff2();
    } catch (Exception $e) {
       //nothing will be caught here, we are an admin
    }

Here you are, you know the basics of AOP.

## AOP Vocabulary and PHP's AOP capabilities ##

### Advice ###

An advice is a piece of code that can be executed. In our first example the function adviceForAdmin is an advice, it
*could* be executed.

In PHP's AOP extension an advice can be a trait, a callback, an anonymous function, a static method of a class,
a method of a given object or a closure.

### Join points ###

Join points are places where we can attach advices.

In PHP's AOP extension, a join point can be:

*    before any method / function call
*    after any method / function call
*    around any method / function call
*    During the arousing of an exception of any method / function
*    after any method / function call, should the method terminate normally or not (triggers an exception or not)

In our first example, we used a "before" join point.

### Pointcut ###

Pointcuts are a way to describe whether or not a given join point will trigger the execution of an advice.

In PHP's AOP extension pointcuts can be configured with a quite simple and straightforward syntax.

In our first example the pointcut was "MyServices->doAdmin*()" and was configured to launch the advice "before" the
execution of the matching methods join points.

## Why or should I use AOP ? ##

AOP is a whole different way of thinking for developing application. It is as different as object oriented programming
 can be opposed to procedural programming.

Even if you don't want to base your future development on this approach, you may find it very useful for debugging
purposes. Imagine a world where you can debug or get information on your code based only on information collected for
a given user, a given context, a given procedure. A world where you can hunt weird and old code execution without even
trying to update multiple and sparse PHP files, but just by adding advices on given conditions.

We are sure that this extension will soon be part of your future development workflow !
