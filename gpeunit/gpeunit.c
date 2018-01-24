#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gpeunit.h"
#include "libgpeunit.h"

typedef struct AnalyseGPEResult_t AnalyseGPEResult;

static int le_gpeunit;
PHP_FUNCTION(confirm_gpeunit_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "gpeunit", arg);
	RETURN_STRINGL(strg, len, 0);
}

PHP_FUNCTION(gpeunit_generate)
{
    char* GPEData = NULL;
    int GPEData_len;
    int seed1;
    int seed2;
    int seed3;
    int seed4;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "sllll",
        &GPEData, &GPEData_len,
        &seed1, &seed2, &seed3, &seed4
    ) == FAILURE) {
        return;
    }

    return;
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
    float RefPointsCountPercent;
    float NoiseCountPercent;
    char* rsMemory;
    int rsMemoryLen;
    void* resultMemory;
    int resultMemorySize;
    AnalyseGPEResult *funcResult;


    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "slllllllllldds",
        &GPEData, &GPEDataLen,
        &seed1, &seed2, &seed3, &seed4,
        &externWhite, &blackBorder, &internWhite, &GPESize, &localShiftsCount, &localShiftSize,
        &RefPointsCountPercent, &NoiseCountPercent, &rsMemory, &rsMemoryLen
        ) == FAILURE) {
            return;
    }

    int res = AnalyseGPE(GPEData, GPEDataLen, externWhite, blackBorder, internWhite, GPESize,
        seed1, seed2, seed3, seed4, localShiftsCount, localShiftSize, RefPointsCountPercent, NoiseCountPercent,
        rsMemory, rsMemoryLen, &resultMemory, &resultMemorySize);
    if (res == 0 && resultMemorySize == sizeof(AnalyseGPEResult)) {
        funcResult = (AnalyseGPEResult*)resultMemory;

        // ...... copy data to your vars ....

        free(funcResult->GPEData);
        free(funcResult);
    }

    return;
};

PHP_MINFO_FUNCTION(gpeunit)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "gpeunit support", "enabled");
	php_info_print_table_end();

}

const zend_function_entry gpeunit_functions[] = {
	PHP_FE(confirm_gpeunit_compiled, NULL) /* For testing, remove later. */
	PHP_FE(gpeunit_generate, NULL)
	PHP_FE(gpeunit_analyze, NULL)
	PHP_FE_END
};

zend_module_entry gpeunit_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_GPEUNIT_EXTNAME,
	gpeunit_functions,
	NULL,				/* MINIT */
	NULL,				/* MSHUTDOWN */
	NULL,				/* RINIT */
	NULL,				/* RSHUTDOWN */
	PHP_MINFO(gpeunit),
	PHP_GPEUNIT_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GPEUNIT
ZEND_GET_MODULE(gpeunit)
#endif
