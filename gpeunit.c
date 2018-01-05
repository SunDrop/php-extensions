/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gpeunit.h"
#include "libgpeunit.h"

/* If you declare any globals in php_gpeunit.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(gpeunit)
*/

/* True global resources - no need for thread safety here */
static int le_gpeunit;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("gpeunit.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_gpeunit_globals, gpeunit_globals)
    STD_PHP_INI_ENTRY("gpeunit.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_gpeunit_globals, gpeunit_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_gpeunit_compiled(string arg)
   Return a string to confirm that the module is compiled in */
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
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_gpeunit_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_gpeunit_init_globals(zend_gpeunit_globals *gpeunit_globals)
{
	gpeunit_globals->global_value = 0;
	gpeunit_globals->global_string = NULL;
}
*/
/* }}} */

/**
 * @param text GPE_Data
 * @param int seed1
 * @param int seed2
 * @param int seed3
 * @param int seed4
 */
PHP_FUNCTION(gpeunit_generate)
{
    char* GPEData = NULL;
    int QRData_len, GPEData_len;
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

    return SUCCESS;
//        int AnalyseGPE(const void* image, int imageSize, int extrnWhite, int blackBorder, int internWhite, int GPESize,
//int seed1, int seed2, int seed3, int seed4, int localShiftsCount, int localShiftSize,
//float RefPointsCountPercent, float NoiceCountPercent, const void* rsMemory, int rsMemorySize,
//void** resultMemory, int* resultMemorySize);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(gpeunit)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(gpeunit)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(gpeunit)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(gpeunit)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(gpeunit)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "gpeunit support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ gpeunit_functions[]
 *
 * Every user visible function must have an entry in gpeunit_functions[].
 */
const zend_function_entry gpeunit_functions[] = {
	PHP_FE(confirm_gpeunit_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(gpeunit_generate, NULL)
	PHP_FE_END	/* Must be the last line in gpeunit_functions[] */
};
/* }}} */

/* {{{ gpeunit_module_entry
 */
zend_module_entry gpeunit_module_entry = {
	STANDARD_MODULE_HEADER,
	"gpeunit",
	gpeunit_functions,
	PHP_MINIT(gpeunit),
	PHP_MSHUTDOWN(gpeunit),
	PHP_RINIT(gpeunit),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(gpeunit),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(gpeunit),
	PHP_GPEUNIT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GPEUNIT
ZEND_GET_MODULE(gpeunit)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
