--TEST--
Try using finfo_open
--FILE--
<?php
$finfo = finfo_open(FILEINFO_MIME_TYPE);
echo finfo_file($finfo, __FILE__);
?>
--EXPECT--
text/x-php
