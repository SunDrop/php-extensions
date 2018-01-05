PHP_ARG_ENABLE(generator,
    [Whether to enable the "generator" extension],
    [  --enable-generator      Enable "generator" extension support])

if test $PHP_GENERATOR != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(GENERATOR_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, GENERATOR_SHARED_LIBADD)
    PHP_ADD_LIBRARY(gpegenerator, 1, GENERATOR_SHARED_LIBADD)
    PHP_NEW_EXTENSION(generator, generator.cc, $ext_shared)
fi