#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gpeunit.h"
#include "libgpeunit.h"

typedef struct AnalyseGPEResult_t AnalyseGPEResult;

static int le_gpeunit;

PHP_FUNCTION(gpeunit_analyze_dump)
{
    char * gpe;
    int gpeLen;
    int seed1;
    int seed2;
    int seed3;
    int seed4;
    int externWhite;
    int blackBorder;
    int internWhite;
    int GPESize;
    int localShiftsCount;
    int localShiftSize;
    double RefPointsCountPercent;
    double NoiseCountPercent;
    zval *zvalRsMemory;
    int rsMemoryLen;
    char *qr;
    int qrLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "sllllllllllddzl|s",
    &gpe, &gpeLen,
    &seed1, &seed2, &seed3, &seed4,
    &externWhite, &blackBorder, &internWhite, &GPESize, &localShiftsCount, &localShiftSize,
    &RefPointsCountPercent, &NoiseCountPercent,
    &zvalRsMemory, &rsMemoryLen,
    &qr, &qrLen
    ) == FAILURE) {
        return;
    }

    php_printf("seed1: %d\n", seed1);
    php_printf("seed2: %d\n", seed2);
    php_printf("seed3: %d\n", seed3);
    php_printf("seed4: %d\n", seed4);
    php_printf("externWhite: %d\n", externWhite);
    php_printf("blackBorder: %d\n", blackBorder);
    php_printf("internWhite: %d\n", internWhite);
    php_printf("GPESize: %d\n", GPESize);
    php_printf("localShiftsCount: %d\n", localShiftsCount);
    php_printf("localShiftSize: %d\n", localShiftSize);
    php_printf("localShiftSize: %d\n", localShiftSize);
    php_printf("RefPointsCountPercent: %f\n", RefPointsCountPercent);
    php_printf("NoiseCountPercent: %f\n", NoiseCountPercent);
    php_printf("QR: %s\n", qr);

    RETURN_BOOL(true);
}

PHP_FUNCTION(gpeunit_analyze)
{
    char* GPEData = NULL;
    int GPEDataLen;
    int seed1;
    int seed2;
    int seed3;
    int seed4;
    int externWhite;
    int blackBorder;
    int internWhite;
    int GPESize;
    int localShiftsCount;
    int localShiftSize;
    double RefPointsCountPercent;
    double NoiseCountPercent;
    zval *zvalRsMemory;
    int rsMemoryLen;
    char *qr;
    int qrLen;

    void* rsMemory;
    void* resultMemory;
    int resultMemorySize;
    AnalyseGPEResult *funcResult;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "sllllllllllddzl|s",
        &GPEData, &GPEDataLen,
        &seed1, &seed2, &seed3, &seed4,
        &externWhite, &blackBorder, &internWhite, &GPESize, &localShiftsCount, &localShiftSize,
        &RefPointsCountPercent, &NoiseCountPercent,
        &zvalRsMemory, &rsMemoryLen,
        &qr, &qrLen
        ) == FAILURE) {
            return;
    }

    rsMemory = emalloc(rsMemoryLen);
    memset(rsMemory, '\0', rsMemoryLen);
    memcpy(rsMemory, Z_STRVAL_P(zvalRsMemory), rsMemoryLen);

    int res = AnalyseGPE(GPEData, GPEDataLen, externWhite, blackBorder, internWhite, GPESize,
        seed1, seed2, seed3, seed4, localShiftsCount, localShiftSize, RefPointsCountPercent, NoiseCountPercent,
        rsMemory, rsMemoryLen, qr, &resultMemory, &resultMemorySize);

    if (res == 0 && resultMemorySize == sizeof(AnalyseGPEResult)) {
        funcResult = (AnalyseGPEResult*)resultMemory;

        // ...... copy data to your vars ....
        array_init(return_value);
        add_assoc_long(return_value, "status", funcResult->status);
        add_assoc_string(return_value, "data", funcResult->GPEData, 1);
        add_assoc_double(return_value, "score", funcResult->score);
        add_assoc_double(return_value, "scoreAdv", funcResult->scoreAdv);
        add_assoc_long(return_value, "shifts", funcResult->localShifts);

        free(funcResult->GPEData);
        free(funcResult);

        return;
    } else {
        RETVAL_STRING((char*)resultMemory, 1);
        free(resultMemory);
        return;
    }

    RETURN_LONG(res);
};

const char * gpeunit_version()
{
    return PHP_GPEUNIT_VERSION;
}

const char * gpeunit_lib_version()
{
    return Version();
}

PHP_FUNCTION(gpeunit_version)
{
    RETVAL_STRING(gpeunit_version(), 0);
}

PHP_FUNCTION(gpeunit_lib_version)
{
    RETVAL_STRING(gpeunit_lib_version(), 0);
}

PHP_MINFO_FUNCTION(gpeunit)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "GPEUnIT support", "enabled");
    php_info_print_table_row( 2, "GPEUnIT Lib version", gpeunit_lib_version());
    php_info_print_table_row( 2, "GPEUnIT PHP Ext version", gpeunit_version());
    php_info_print_table_row( 2, "GPEUnIT PHP Ext Revision", "$Id$");
    php_info_print_table_end();
}

const zend_function_entry gpeunit_functions[] = {
    PHP_FE(gpeunit_analyze, NULL)
    PHP_FE(gpeunit_analyze_dump, NULL)
    PHP_FE(gpeunit_version, NULL)
    PHP_FE(gpeunit_lib_version, NULL)
    PHP_FE_END
};

zend_module_entry gpeunit_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_GPEUNIT_EXTNAME,
    gpeunit_functions,
    NULL,                /* MINIT */
    NULL,                /* MSHUTDOWN */
    NULL,                /* RINIT */
    NULL,                /* RSHUTDOWN */
    PHP_MINFO(gpeunit),
    PHP_GPEUNIT_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GPEUNIT
ZEND_GET_MODULE(gpeunit)
#endif
