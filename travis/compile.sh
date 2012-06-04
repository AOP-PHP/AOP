#!/bin/bash

sh -c "phpize && ./configure && make && sudo make install"
PHPINI=`php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
#echo $PHPINI;
#PHPEXE=`echo $PHPINI | sed -e "s|etc/php.ini|bin/php|"`
PHPEXE=`php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||" | sed -e "s|etc/php.ini|bin/php|"`
echo $PHPEXE;
echo $TEST_PHP_EXECUTABLE;
echo "extension=AOP.so" >> $PHPINI
