--TEST--
Absolute namespaces in functions
--FILE--
<?php
namespace Mon\Plugin {

      function get () {
         echo "get";
      }
   
}
namespace {
   aop_add_before ('Mon\Plugin\get()', function(){echo "not root"; });
   aop_add_before ('\Mon\Plugin\get()', function(){echo "root";});
   Mon\Plugin\get();
}
?>
--EXPECT--
not rootrootget
