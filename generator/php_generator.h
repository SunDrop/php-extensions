#ifndef PHP_GENERATOR_H
#define PHP_GENERATOR_H

#define PHP_GENERATOR_EXTNAME  "generator"
#define PHP_GENERATOR_EXTVER   "1.0"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

extern "C" {
#include "php.h"
}

extern zend_module_entry generator_module_entry;
#define phpext_generatorgpe_ptr &generator_module_entry;

#endif /* PHP_GENERATOR_H */