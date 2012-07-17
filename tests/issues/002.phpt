--TEST--
Testing that the use of static is still enabled
--FILE--
<?php
class Index 
{
   public function update ($index) {
      static::doStuff();
   }

   static public function doStuff() {
      echo "I'm index";
   }
}

$index = new Index();
$index->update(1);
?>
--EXPECT--
I'm index
