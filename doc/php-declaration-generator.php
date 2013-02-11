<?php

define('T', '    ');
define('N', PHP_EOL);

$functions = array('aop_add_before', 'aop_add_after', 'aop_add_around');
$classes = array('AopJoinPoint');
$constant_prefix = 'AOP';

$php = '<?php' . N;
$php .= '/**' . N . ' * Generated stub file for code completion purposes' . N . ' */';
$php .= N . N;

//echo ReflectionClass::export('ReflectionMethod');
foreach (get_defined_constants() as $cname => $cvalue) {
    if (strpos($cname, $constant_prefix) === 0) {
        $php .= 'define(\'' . $cname . '\', ' . $cvalue . ');' . N;
    }
}

$php .= N;

foreach ($functions as $function) {
    $refl = new ReflectionFunction($function);
    $php .= 'function ' . $refl->getName() . '(';
    foreach ($refl->getParameters() as $i => $parameter) {
        if ($i >= 1) {
            $php .= ', ';
        }
        if ($typehint = $parameter->getClass()) {
            $php .= $typehint->getName() . ' ';
        }
        $php .= '$' . $parameter->getName();
        if ($parameter->isDefaultValueAvailable()) {
            $php .= ' = ' . $parameter->getDefaultValue();
        }
    }
    $php .= ') {}' . N;
}

$php .= N;

foreach ($classes as $class) {
    $refl = new ReflectionClass($class);
    $php .= 'class ' . $refl->getName();
    if ($parent = $refl->getParentClass()) {
        $php .= ' extends ' . $parent->getName();
    }
    $php .= N . '{' . N;
    foreach ($refl->getProperties() as $property) {
        $php .= T . '$' . $property->getName() . ';' . N;
    }
    foreach ($refl->getMethods() as $method) {
        if ($method->isPublic()) {
            if ($method->getDocComment()) {
                $php .= T . $method->getDocComment() . N;                
            }
            $php .= T . 'public function ';
            if ($method->returnsReference()) {
                $php .= '&';
            }
            $php .= $method->getName() . '(';
            foreach ($method->getParameters() as $i => $parameter) {
                if ($i >= 1) {
                    $php .= ', ';
                }
                if ($parameter->isArray()) {
                    $php .= 'array ';
                }
                if ($typehint = $parameter->getClass()) {
                    $php .= $typehint->getName() . ' ';
                }
                $php .= '$' . $parameter->getName();
                if ($parameter->isDefaultValueAvailable()) {
                    $php .= ' = ' . $parameter->getDefaultValue();
                }
            }
            $php .= ') {}' . N;
        }
    }
    $php .= '}';
}

echo $php . N;

