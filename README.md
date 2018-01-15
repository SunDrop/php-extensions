# Extension's
* analyser - v1.0
* generator - v1.0
* gpeunit - v2.0

## Clone repo
```
git clone -b php_exts --single-branch git@gitlab.unit.lan:unit/gze.git
```

## Requirements
* php v 5.6.*
* phpize (the same version)

## Build
```
1. git clone git@github.com:php/php-src.git php-src
2. git checkout PHP-5.6.30
3. cd php-src/ext
4. git clone -b php_exts --single-branch git@gitlab.unit.lan:unit/gze.git
5. cd <ext_name>
6. phpize --clean
7. phpize
8. ./configure
9. make install
```

## Create|Update extension
### generator
```
1. $ cd ..
2. $ vi ext/generator/config.m4
3. $ ./buildconf
4. $ ./configure --[with|enable]-generator
5. $ make
6. $ ./sapi/cli/php -f ext/generator/generator.php
7. $ vi ext/generator/generator.cc
8. $ make
```
### analyser
```
1. $ cd ..
2. $ vi ext/analyser/config.m4
3. $ ./buildconf
4. $ ./configure --[with|enable]-analyser
5. $ make
6. $ ./sapi/cli/php -f ext/analyser/analyser.php
7. $ vi ext/analyser/generator.cc
8. $ make
```
### gpeunit
```
1. $ cd ..
2. $ vi ext/gpeunit/config.m4
3. $ ./buildconf
4. $ ./configure --[with|enable]-gpeunit
5. $ make
6. $ ./sapi/cli/php -f ext/gpeunit/gpeunit.php
7. $ vi ext/gpeunit/gpeunit.c
8. $ make
```