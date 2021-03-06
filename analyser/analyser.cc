#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_analyser.h"
#include <stdint.h>
#include <inttypes.h>
#include <ug_qa.h>

PHP_FUNCTION(analyser_initialize)
{
    const void* protoStructIn;
    size_t protoStructInLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "s", &protoStructIn, &protoStructInLen) == FAILURE) return;

    int initialize_result = Initialize(protoStructIn, protoStructInLen);

    if (initialize_result == 0) {
        RETURN_BOOL(1);
    } else {
        RETURN_BOOL(0);
    }
}

PHP_FUNCTION(analyser_lib_version)
{
    float version = Version();
    RETURN_DOUBLE(version);
}

PHP_FUNCTION(analyser_lib_version_text)
{
    const char* version = VersionText();
    RETURN_STRING(version, 1);
}

const char * analyser_version()
{
    return PHP_ANALYSER_EXTVER;
}

const char * analyser_lib_version()
{
    return VersionText();
}


PHP_FUNCTION(analyser_decode_qr_memory)
{
    char* protoStructIn;
    void* protoStructOutStr;
    int protoStructInLen;
    void* protoStructOut;
    uint32_t protoStructOutLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "s", &protoStructIn, &protoStructInLen) == FAILURE) return;

    int decodeResult = DecodeQRMemory(protoStructIn, protoStructInLen, &protoStructOut, &protoStructOutLen);

    if (decodeResult != 0) RETURN_LONG(decodeResult);

    protoStructOutStr = emalloc(protoStructOutLen);
    memcpy(protoStructOutStr, protoStructOut, protoStructOutLen);
    free(protoStructOut);

    RETURN_STRINGL((const char*) protoStructOutStr, protoStructOutLen, 0);
}

PHP_FUNCTION(analyser_decode_qr)
{
    char* protoStructIn;
    void* protoStructOutStr;
    int protoStructInLen;
    void* protoStructOut;
    uint32_t protoStructOutLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "s", &protoStructIn, &protoStructInLen) == FAILURE) return;

    int decodeResult = DecodeQR(protoStructIn, protoStructInLen, &protoStructOut, &protoStructOutLen);

    if (decodeResult != 0) RETURN_LONG(decodeResult);

    protoStructOutStr = emalloc(protoStructOutLen);
    memcpy(protoStructOutStr, protoStructOut, protoStructOutLen);
    free(protoStructOut);

    RETURN_STRINGL((const char*) protoStructOutStr, protoStructOutLen, 0);
}

PHP_FUNCTION(analyser_analys_image_memory)
{
    char* protoStructIn;
    void* protoStructOutStr;
    int protoStructInLen;
    void* protoStructOut;
    uint32_t protoStructOutLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "s", &protoStructIn, &protoStructInLen) == FAILURE) return;

    int decodeResult = AnalysImageMemory(protoStructIn, protoStructInLen, &protoStructOut, &protoStructOutLen);

    if (decodeResult != 0) RETURN_LONG(decodeResult);

    protoStructOutStr = emalloc(protoStructOutLen);
    memcpy(protoStructOutStr, protoStructOut, protoStructOutLen);
    free(protoStructOut);

    RETURN_STRINGL((const char*) protoStructOutStr, protoStructOutLen, 0);
}

PHP_FUNCTION(analyser_analys_image)
{
    char* protoStructIn;
    void* protoStructOutStr;
    int protoStructInLen;
    void* protoStructOut;
    uint32_t protoStructOutLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS( ) TSRMLS_CC, "s", &protoStructIn, &protoStructInLen) == FAILURE) return;

    int decodeResult = AnalysImage(protoStructIn, protoStructInLen, &protoStructOut, &protoStructOutLen);

    if (decodeResult != 0) RETURN_LONG(decodeResult);

    protoStructOutStr = emalloc(protoStructOutLen);
    memcpy(protoStructOutStr, protoStructOut, protoStructOutLen);
    free(protoStructOut);

    RETURN_STRINGL((const char*) protoStructOutStr, protoStructOutLen, 0);
}

PHP_MINFO_FUNCTION(analyser)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Analyser support", "enabled");
    php_info_print_table_row( 2, "Analyser Lib version", analyser_lib_version());
    php_info_print_table_row( 2, "Analyser PHP Ext version", analyser_version());
    php_info_print_table_end();
}

static zend_function_entry analyser_functions[] = {
    PHP_FE(analyser_initialize, NULL)
    PHP_FE(analyser_lib_version, NULL)
    PHP_FE(analyser_lib_version_text, NULL)
    PHP_FE(analyser_decode_qr_memory, NULL)
    PHP_FE(analyser_decode_qr, NULL)
    PHP_FE(analyser_analys_image_memory, NULL)
    PHP_FE(analyser_analys_image, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry analyser_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_ANALYSER_EXTNAME,
    analyser_functions, /* Functions */
    NULL,                /* MINIT */
    NULL,                /* MSHUTDOWN */
    NULL,                /* RINIT */
    NULL,                /* RSHUTDOWN */
    PHP_MINFO(analyser), /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ANALYSER_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ANALYSER
extern "C" {
ZEND_GET_MODULE(analyser)
}
#endif