#ifndef PHP_ANALYSER_H
#define PHP_ANALYSER_H

#define PHP_ANALYSER_EXTNAME  "analyser"
#define PHP_ANALYSER_EXTVER   "1.1.0"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

extern "C" {
#include "php.h"
}

extern zend_module_entry analyser_module_entry;
#define phpext_analyser_ptr &analyser_module_entry;

#endif /* PHP_ANALYSER_H */