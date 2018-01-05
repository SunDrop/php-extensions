#include "php_generator.h"
#include "gpegenerator/include/gpegenerator.h"
#include <stdint.h>
#include <inttypes.h>

PHP_FUNCTION(generate_gpe)
{
	char* szQRData = NULL;
	char* cbGPEData = NULL;
	int szQRData_len, cbGPEData_len;
	int seed1;
	int seed2;
	int seed3;

	if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "lllss", &seed1, &seed2, &seed3, &szQRData, &szQRData_len, &cbGPEData, &cbGPEData_len) == FAILURE) return;
	
	uint8_t* pOut=new uint8_t[1024*1024];
	uint32_t dwOutSize=1024*1024;

	uint32_t dwRet = GPE_generate(seed1, seed2, seed3, (uint8_t*)szQRData, szQRData_len, (uint8_t*)cbGPEData, cbGPEData_len, pOut, &dwOutSize, 1);

	if (dwRet != GPE_ERROR_SUCCESS) RETURN_LONG(dwRet);

	array_init(return_value);
	for (int i = 0; i < dwOutSize; i++) add_index_long(return_value, i, pOut[i]);
	add_next_index_long(return_value, dwOutSize);
	

	return;
}

PHP_MINIT_FUNCTION(generator)
{
    return SUCCESS;
}

static zend_function_entry generator_functions[] = {
    PHP_FE(generate_gpe, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry generator_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_GENERATOR_EXTNAME,
    generator_functions, /* Functions */
    PHP_MINIT(generator), /* MINIT */
    NULL,				/* MSHUTDOWN */
    NULL,				/* RINIT */
    NULL,				/* RSHUTDOWN */
    NULL,				/* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_GENERATOR_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GENERATOR
extern "C" {
ZEND_GET_MODULE(generator)
}
#endif