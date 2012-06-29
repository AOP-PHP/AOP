--TEST--
Docs Example 2
--FILE--
<?php 
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

aop_add_before('MyServices::doStuff()', 'adviceToInterruptDoStuff');

$services = new MyServices();
try {
    $services->doStuff();
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
I will never do that with someone I don't know
