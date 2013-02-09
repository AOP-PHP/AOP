/*
/+----------------------------------------------------------------------+
 | AOP                                                                  |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012 Julien Salleyron, Gérald Croës                    |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt.                                 |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Julien Salleyron <julien.salleyron@gmail.com>                |
 +----------------------------------------------------------------------+
*/
#ifndef PHP_AOP_JOINPOINT_H
#define PHP_AOP_JOINPOINT_H 1

PHP_METHOD(AopJoinpoint, getArguments);
PHP_METHOD(AopJoinpoint, getPropertyName);
PHP_METHOD(AopJoinpoint, getPropertyValue);
PHP_METHOD(AopJoinpoint, setArguments);
PHP_METHOD(AopJoinpoint, getKindOfAdvice);
PHP_METHOD(AopJoinpoint, getReturnedValue);
PHP_METHOD(AopJoinpoint, getAssignedValue);
PHP_METHOD(AopJoinpoint, setReturnedValue);
PHP_METHOD(AopJoinpoint, setAssignedValue);
PHP_METHOD(AopJoinpoint, getPointcut);
PHP_METHOD(AopJoinpoint, getObject);
PHP_METHOD(AopJoinpoint, getClassName);
PHP_METHOD(AopJoinpoint, getMethodName);
PHP_METHOD(AopJoinpoint, getFunctionName);
PHP_METHOD(AopJoinpoint, getException);
PHP_METHOD(AopJoinpoint, process);

#endif
