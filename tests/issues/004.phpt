--TEST--
Exception propagation is not correct
--FILE--
<?php
    class AdminDeniedException extends Exception {}
    function doAdminStuff(){
        echo "do admin stuff\n";
    }
    function adviceForDoAdmin(){
        global $role;
        if ($role !== 'admin') {
            throw new AdminDeniedException();
        }
    }
        
    function adviceForException($obj){
        $method = $obj->getFunctionName();
        try {
            $obj->process();
        } catch(AdminDeniedException $e) {
            echo "no privilige to access $method\n";
        }
    }

    aop_add_before('doAdmin*()' , 'adviceForDoAdmin');
    aop_add_around('doAdmin*()', 'adviceForException');

    try {
        doAdminStuff();
    } catch(Exception $e) {
        echo "unknown exception"; 
    }
?>
--EXPECT--
unknown exception
