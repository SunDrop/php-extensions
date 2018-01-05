PHP_ARG_ENABLE(analyser,
    [Whether to enable the "analyser" extension],
    [  --enable-analyser      Enable "analyser" extension support])

if test $PHP_ANALYSER != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(ANALYSER_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, ANALYSER_SHARED_LIBADD)
    PHP_ADD_LIBRARY(ug_qa_lib, 1, ANALYSER_SHARED_LIBADD)
    PHP_NEW_EXTENSION(analyser, analyser.cc, $ext_shared)
fi