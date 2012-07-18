--TEST--
Exceptions thrown by a before is caught by an around
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
            echo "no privilege to access $method\n";
        }
    }

    aop_add_around('doAdmin*()', 'adviceForException');
    aop_add_before('doAdmin*()' , 'adviceForDoAdmin');

    try {
        doAdminStuff();
    } catch(Exception $e) {
        echo "unknown exception"; 
    }
?>
--EXPECT--
no privilege to access doAdminStuff
