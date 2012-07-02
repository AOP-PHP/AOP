--TEST--
An advice that is interested only in properties writing
--FILE--
<?php
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
?>
--EXPECT--
Celebrity secretly act on something !

