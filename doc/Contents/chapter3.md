# Advanced topics #

## Advice triggered by multiple pointcuts on the same joinpoint ##

If an advice is registered mupliple times for a given joinpoint that is matched by several pointcuts, will it be
triggered multiple times ?

It depends on the third optionnal parameter you specified in aop_add_*.

*    Aop::SINGLE (DEFAULT) the pointcut registered with Aop::SINGLE won't trigger the advice one
more time if the advice was launched before by another pointcut .
*    Aop::MULTIPLE => The pointcut registered with Aop::MULTIPLE will trigger the advice one
more time, even if the advice was launched before by any other pointcut.

Example of multiple launch of a given advice

    [php]
    aop_add_around('Lib::*', $logAdvice);
    aop_add_before('*', $logAdvice, Aop::MULTIPLE);//will be launched even for Lib::*

Example of a single launch of a given advice

    [php]
    aop_add_around('Lib::*', $logAdvice);
    aop_add_before('*', $logAdvice, Aop::SINGLE);//won't be launched for Lib::* as the same advice has been launched before

## In wich orer will my pointcuts / advice will be resolved ? ##

The advices are executed in the registration order.

## Declaring new joinpoints ##

TODO

    [php]
    function maMethode ()
    {
        aop_joinpoint('id', $params, $params);

        //could enables the developers to create join points from nowhere, for log purposes
        or events
    }