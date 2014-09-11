#ifndef ROXML_INTERNAL_H
#define ROXML_INTERNAL_H

#define ROXML_INT
#ifdef __DEBUG
#define ROXML_STATIC
#else
#define ROXML_STATIC static
#endif

/* STUB function are here to provide a consistent libroxml binary no matter 
 * what the configuration. They are located in a specific section that can be 
 * easily optimized out if necessary */
#define ROXML_STUB __attribute__((weak)) __attribute__((section("__stub")))

/* must be included first */
#include <roxml_tune.h>

#include <roxml_types.h>
#include <roxml_defines.h>

/* must be included last */
#include <roxml.h>

#endif /* ROXML_INTERNAL_H */
