--TEST--
Try using new finfo()
--FILE--
<?php
class mytest {
    public function test() {
        return new finfo(FILEINFO_MIME_TYPE);
    }
}

$test = new mytest();
echo $test->test()->file(__FILE__);
?>
--EXPECTF--
text/x-%s
