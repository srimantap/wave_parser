/*
 * 
 * Copyright (C) 2020  Bits For Bytes <support@bitsforbyte.com>
 *
 * log.c: logging functionalilies 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "log.h"
 
static int log_level = LOG_DEFAULT;


void log_printf(int level, const char *fmt, ...)
{
  va_list ap;

  if (log_level < level)
    return;

  /* draw out the vararg format */
  va_start(ap, fmt);

  if (log_level > 0)
    vfprintf(stderr, fmt, ap);

  va_end(ap);

  return;
}

int log_set_level(const char *str)
{
  char *endptr;
  long val;

  val = strtol(str, &endptr, 10);

  /* Check for various possible errors */

  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)) {
    log_warning("strtol error");
    return 1;
  }

  if (endptr == str) {
    log_warning("No digits were found\n");
    return 1;
  }

  if (*endptr != '\0') {        /* Not necessarily an error... */
    log_warning("Further characters after number: %s\n", endptr);
    return 1;
  }

  if (val < 0 || val > LOG_MAX) {        /* Not necessarily an error... */
    log_warning("Invalid verbose level, (min = 1, max = %d), selected %d\n", LOG_MAX, val);
    return 1;
  }

  /* If we got here, strtol() successfully parsed a number */
  log_level = val;
  log_debug("Verbose level = %ld\n", val); 

  return 0;
}

