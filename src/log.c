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
#include <time.h>

#include "log.h"

#define LOG_TIME_LENGTH      25
#define LOG_TIME_FORMAT      "%Y-%m-%d %H:%M:%S"
#define LOG_LEVEL_LENGTH     10
#define LOG_FILENAME_LENGTH  30
#define LOG_MESSAGE_LENGTH   200

static int log_level = LOG_DEFAULT;

/* Log level strings */
static const char *log_strings[] = {
  "NONE", /* to match with enum, none = 0" */
  "MESSAGE",
  "ERROR",
  "WARN",
  "DEBUG",
  "INFO",
};

static const char *log_colors[] = {
  "NONE", /* to match with enum, none = 0" */
  "\x1b[37m",
  "\x1b[31m",
  "\x1b[33m",
  "\x1b[36m",
  "\x1b[32m",
};

void log_printf(int level, const char *fmt, ...)
{
  //char log_line[1024];

  char log_time[LOG_TIME_LENGTH];
  char log_type[LOG_LEVEL_LENGTH];
  //char log_filename[LOG_FILENAME_LENGTH];
  char log_message[LOG_MESSAGE_LENGTH];

  va_list ap;
  time_t t;
  struct tm *tmp;

  if (log_level < level)
      return;

  /* print the time */
  t = time(NULL);
  tmp = localtime(&t);
  if (tmp == NULL) {
    perror("localtime");
    return;
  }

  if (strftime(log_time, sizeof(log_time), LOG_TIME_FORMAT, tmp) == 0) {
    fprintf(stderr, "strftime returned 0");
    return;
  }


  /* print the level info */
  snprintf(log_type, LOG_LEVEL_LENGTH, "%s", log_strings[level]);

  va_start(ap, fmt);

  if (log_level > 0)
    vsnprintf(log_message, LOG_MESSAGE_LENGTH, fmt, ap);

  fprintf(stderr, "%-25s %s%-10s\x1b[0m %-80s", log_time, log_colors[level], log_type, log_message);
  fprintf(stderr, "\n");

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
    log_warning("No digits were found");
    return 1;
  }

  if (*endptr != '\0') {        /* Not necessarily an error... */
    log_warning("Further characters after number: %s", endptr);
    return 1;
  }

  if (val < 0 || val > LOG_MAX) {        /* Not necessarily an error... */
    log_warning("Invalid verbose level, (min = 1, max = %d), selected %d\n", LOG_MAX, val);
    return 1;
  }

  /* If we got here, strtol() successfully parsed a number */
  log_level = val;
  log_debug("Verbose level = %ld", val);

  return 0;
}

