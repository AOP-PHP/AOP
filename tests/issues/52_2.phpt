--TEST--
Absolute namespaces
--FILE--
<?php
namespace Mon\Plugin {
   class Config { 
      public function get () {
         echo "get";
      }
   }
}
namespace {
   aop_add_before ('Mon\Plugin\Conf*->get*()', function(){echo "not root"; });
   aop_add_before ('\Mon\Plugin\Conf*->get*()', function(){echo "root";});
   $config = new Mon\Plugin\Config();
   $config->get();
}
?>
--EXPECT--
not rootrootget
