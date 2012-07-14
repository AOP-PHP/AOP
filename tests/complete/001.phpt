--TEST--
Caching example with multiple objects with advices on
--FILE--
<?php
    class Services
    {
        public function doGetDataOn ($idData)
        {
            //Du code compliqué et lent de récupération de données
            echo "Calculating doGetDataOn($idData)\n";
            return "doGetDataOn($idData)\n";
        }
        public function doGetDataOn2 ($idData2)
        {
            //encore du code compliqué et lent de récupération de données
            echo "Calculating doGetDataOn2($idData2)\n";
            return "doGetDataOn2($idData2)\n";
        }
    }

    //création de la fonction, "ce serait bien si"
    $conseil = function (AopJoinPoint $atjp) {
        //Le cache sera réalisé sous la forme d'une variable statique
        static $cache = array();

        //On récupère les arguments envoyés à la méthode doGetDataQqChose
        $callArguments = $atjp->getArguments();
        $idData = $callArguments[0];

        //On récupère le nom de la méthode appelée (doGetDataQqchose)
        $calledMethodName = $atjp->getMethodName();

        //vérifie s'il existe un cache pour l'appel courant
        if (isset($cache[$calledMethodName][$idData])) {
           return $cache[$calledMethodName][$idData];
        } else {
           //Le cache n'existe pas, on exécute la méthode d'origine
           $cache[$calledMethodName][$idData] = $atjp->process();        }
    }; 

    aop_add_around('Services->doGetDataOn*()', $conseil);

    $service1 = new Services();
    echo $service1->doGetDataOn(1);
    $service2 = new Services();
    echo $service2->doGetDataOn2(1);
    //Now the cache is loaded for bot doGetDataOn(1) and doGetDataOn2(1)
    echo $service1->doGetDataOn2(1);
    echo $service2->doGetDataOn(1);
    //Making another call on doGetDataOn(2)
    echo $service2->doGetDataOn(2);
    echo $service1->doGetDataOn(2);
?>
--EXPECT--
Calculating doGetDataOn(1)
doGetDataOn(1)
Calculating doGetDataOn2(1)
doGetDataOn2(1)
doGetDataOn2(1)
doGetDataOn(1)
Calculating doGetDataOn(2)
doGetDataOn(2)
doGetDataOn(2)
