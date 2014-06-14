--TEST--
Try using finfo_open
--SKIPIF--
<?php
if (!function_exists('finfo_open') {
    die ('You need finfo');
}
?>
--FILE--
<?php
$finfo = finfo_open(FILEINFO_MIME_TYPE);
echo finfo_file($finfo, __FILE__);
?>
--EXPECT--
text/plain
