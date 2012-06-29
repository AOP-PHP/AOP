--TEST--
Docs Test 15
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
       //You're NOT asking explicitely for the reference
       $result = $joinpoint->getReturnedValue();
       //The returned value of the trigerring method won't be updated
       $result = "This is the new text";
    };

    aop_add_after("Writer::getText()", $advice);

    $writer = new Writer();
    $text = $writer->getText();
    $writer->echoText();

?>
--EXPECT--
some text

