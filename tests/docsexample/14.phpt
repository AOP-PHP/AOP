--TEST--
Docs Test 14
--FILE--
<?php    class Writer
    {
       protected $text;

       public function & getText ()
       {
          $this->text = "some text";
          return $this->text;
       }

       public function echoText ()
       {
          echo $this->text;
       }
    }

    $advice = function (AopTriggeredJoinpoint $joinpoint) {
       //You're asking explicitely for the reference
       $result = & $joinpoint->getReturnedValue();
       //Updating the value of the reference
       $result = "This is the new text";
    };

    aop_add_after("Writer::getText", $advice);

    $writer = new Writer();
    $text = $writer->getText();
    $writer->echoText();

?>
--EXPECT--
This is the new text

