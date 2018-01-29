PHP_ARG_ENABLE(gpeunit,
    [whether to enable gpeunit support],
    [  --enable-gpeunit           Enable gpeunit support])

if test "$PHP_GPEUNIT" != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(GPEUNIT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, GPEUNIT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(gpelib, 1, GPEUNIT_SHARED_LIBADD)
    PHP_NEW_EXTENSION(gpeunit, gpeunit.cc, $ext_shared)
fi
