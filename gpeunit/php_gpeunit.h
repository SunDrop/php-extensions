#ifndef PHP_GPEUNIT_H
#define PHP_GPEUNIT_H

extern zend_module_entry gpeunit_module_entry;
#define phpext_gpeunit_ptr &gpeunit_module_entry

#define PHP_GPEUNIT_EXTNAME "gpeunit" /* Replace with version number for your extension */
#define PHP_GPEUNIT_VERSION "2.0.3" /* Replace with version number for your extension */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHP_WIN32
#	define PHP_GPEUNIT_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GPEUNIT_API __attribute__ ((visibility("default")))
#else
#	define PHP_GPEUNIT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define GPEUNIT_G(v) TSRMG(gpeunit_globals_id, zend_gpeunit_globals *, v)
#else
#define GPEUNIT_G(v) (gpeunit_globals.v)
#endif

#endif	/* PHP_GPEUNIT_H */