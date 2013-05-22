--TEST--
Try using finfo_open (may cause segmentation fault)
--SKIPIF--
<?php
if (!function_exists('finfo_open') {
    die ('You need finfo');
}
?>
--FILE--
<?php
class mytest {
    public function test() {
        return finfo_open(FILEINFO_MIME_TYPE);
    }
}

$test = new mytest();
$test->test();
echo 'PASS';
?>
--EXPECT--
PASS
