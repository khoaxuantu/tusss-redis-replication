#include "stdio.h"
#include "stdlib.h"
#include "errno.h"

#if !defined(HELPERS_H)
#define HELPERS_H

/**
 * Print an error message and exit the program
 */
void die(const char *str)
{
  perror(str);
  exit(EXIT_FAILURE);
}

#endif
