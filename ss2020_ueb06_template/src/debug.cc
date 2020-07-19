/*
 * debug.c
 *
 *  Created on: Apr 18, 2020
 *      Author: fbeenen
 */


#include <stdio.h>

#include "debug.h"
#include <stdarg.h>

int dbgPrint(const char *temp, ...) {
  va_list ap;
  int result;

  va_start(ap, temp);
  result = vfprintf(stdout, temp, ap);
  va_end(ap);
  return result;
}

