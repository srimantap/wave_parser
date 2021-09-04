/*
 * 
 * Copyright (C) 2020  Bits For Bytes <support@bitsforbyte.com>
 *
 * log.h: Header file logging function
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

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>

#define LOG_DEFAULT    LOG_WARNING
#define LOG_MAX        LOG_INFO

enum {
  LOG_MESSAGE = 1,
  LOG_ERROR,
  LOG_WARNING,
  LOG_DEBUG,
  LOG_INFO,
};

#define log_message(format, ...) \
  do { \
    log_printf(LOG_MESSAGE, format, ##__VA_ARGS__); \
  } while(0)

#define log_error(format, ...) \
  do { \
    log_printf(LOG_ERROR, format, ##__VA_ARGS__); \
  } while(0)

#define log_warning(format, ...) \
  do { \
    log_printf(LOG_WARNING, format, ##__VA_ARGS__); \
  } while(0)

#define log_debug(format, ...) \
  do { \
    log_printf(LOG_DEBUG, format, ##__VA_ARGS__); \
  } while(0)

#define log_info(format, ...) \
  do { \
    log_printf(LOG_INFO, format, ##__VA_ARGS__); \
  } while(0)


int log_set_level(const char *val);
void log_printf(int level, const char *fmt, ...);

#endif /* End of __LOG_H__ */
