#! /bin/sh

# INC=-I./cryptounit-dstu
# CXXFLAGS="-DPHP7 -std=c++11 -Wno-narrowing $INC"
# export CXXFLAGS
# LDFLAGS="-L/usr/lib64"
# export LDFLAGS
# LD_LIBRARY_PATH=/usr/lib64
# export LD_LIBRARY_PATH

phpize --clean
phpize
./configure
make install

#1.  $ cd ..
#2.  $ vi ext/gpeunit/config.m4
#3.  $ ./buildconf
#4.  $ ./configure --[with|enable]-gpeunit
#5.  $ make
#6.  $ ./sapi/cli/php -f ext/gpeunit/gpeunit.php
#7.  $ vi ext/gpeunit/gpeunit.c
#8.  $ make

