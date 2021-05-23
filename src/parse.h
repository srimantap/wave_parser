/*
 *
 * Copyright (C) 2020  Bits For Bytes <support@bitsforbyte.com>
 *
 * parse.h: Header file to include for wave parser
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

#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdio.h>
#include <stdint.h>

/* Error codes */
enum error_code {
  ERROR_INVALID_FD         = -1,
  ERROR_INVALID_SIZE       = -2,
  SUCESS                   = 0,
};

/* Header sizes */
enum wave_fmt_size {
  SIZE_CHUNK_ID = 4,
};

struct wave_format {

  uint8_t chunk_id[4];

};

/*
 *
 */
int parse_wave(char *wavefile);


#endif /* end of __PARSE_H__ */
