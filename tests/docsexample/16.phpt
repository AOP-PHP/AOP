--TEST--
An advice that knows you're dealing with an object's property
--FILE--
<?php
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

    aop_add_before('Celebrity->*Stuff', $paparazzi_informer);
    
    $CynthiaBellulla = new Celebrity();
    $CynthiaBellulla->act();
    echo $CynthiaBellulla->say();
    
--EXPECT--
Celebrity will act or say something !Celebrity will act or say something !public thinking
