--TEST--
An advice that knows what happened on the hooked properties
--FILE--
<?php
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
          $paparazzi->shoot($aop_tjp->getTriggeringObject()->getName(),
                            $aop_tjp->getAssignedValue(),
                            $aop_tjp->getTriggeringPropertyName()
                            );
       }
    };

    //will be triggered before writing the property privateStuff
    aop_add_before('Celebrity->secret', $paparazzi_informer);

    $CynthiaBellulla = new Celebrity('Cynthia Bellula');
    $CynthiaBellulla->act();
?>
--EXPECT--
I'm taking pictures of Cynthia Bellula doing some shoping at london (that's supposed to be secret)
