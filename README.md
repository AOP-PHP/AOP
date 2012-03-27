AOP for PHP
=======================

PHP Requirements
----------------

 * PHP 5.3 - not sure exact version
 * or PHP 5.4

Why use AOP
======================

AOP is an extension for AOP in PHP

How it works
============

You can around method execution with the function "AOP_add"

You must specified 2 parameters.

First a selector for describe what call you want to around.
Second a callback which will be called when a method matching the previous selector is called

The callback receive an object as parameter which have 5 methods

 * getFunctionName() : return the name of the method call (in lowercase) (ns\class::method)
 * getArgs() : return an array of the parameter passed to the called method
 * getThis() : return the current object of the method which has been caught
 * process() : execute the catch method and return its result
 * processWithArgs(array $pArgs) : execute the catch method replacing original args by the $pArgs and return is result

The callback return is the return you will give to the result of the catched method

The catch selector syntax

 * "A::test"
 * "A::*"
 * "A::admin*"
 * "A::*admin"
 * "ns1\ns2\ns3\A::method"
 * "ns1\ns2\*\A::method"
 * "*\ns2\ns3\A::method"
 * "ns1\*\ns3\A::method"
 * "ns*\*2\ns3\A::method"
 * "ns1\**\A::method"
 * "public A::method"
 * "protected|private A::method"
 * "static A::method"
 * "!static A::method"


Examples
========

<?php 
class MyClass {
    public function adminPart () {

    }
    public function adminPart2() {

    }
}

AOP_add ("public MyClass::admin*", function ($pObj) {
   echo "You call an admin part";
   return $pObj->process();
});

$test = new MyClass ();
$test->adminPart();
$test->adminPart2();
?>

