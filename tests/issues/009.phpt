--TEST--
Issue 37 on GitHub
--FILE--
<?php

class Form {

    public function render(array $aParams = array()) {
        return 'foo';
    }
}

class Csrf {

    public function aroundFormRender(\AopJoinpoint $joinpoint) {
        # $joinpoint->process();
    }
}

$oForm = new Form();
$oCsrf = new Csrf();
aop_add_around('Form->render()', array($oCsrf, 'aroundFormRender'));
echo $oForm->render();
echo 'bar';
--EXPECT--
bar
