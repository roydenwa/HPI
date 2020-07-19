/*
 * debug.h
 *
 *  Created on: Apr 18, 2020
 *      Author: fbeenen
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

/** Flag, if Debug-Output should be generated */
#define VERBOSE

/**
 * INFO-Macro for debugging purposes.
 * Usage is the same as printf.
 * Here: INFO(("Hello %d\n", 1));
 */
#ifdef VERBOSE
#define INFO(ARGS)      \
  (dbgPrint("INFO    <%s[%i]>: ", __FILE__, __LINE__), dbgPrint ARGS)
#else
#define INFO(ARGS)
#endif

#define DEBUG (printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__))


/**
 * Helper-function which must not be used explicitly.
 * Use INFO-Macro instead.
 *
 * @return return value of vfprintf.
 */
extern "C" int dbgPrint (const char *temp, ...);

#endif /* DEBUG_H_ */
