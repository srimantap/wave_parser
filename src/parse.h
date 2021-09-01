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
enum wave_endieness {
  LITTLE_ENDIEN = 0,
  BIG_ENDIEN    = 1,
};

/* Header sizes */
enum wave_fmt_size {
  SIZE_CHUNK_ID         = 4,
  SIZE_CHUNK_SIZE       = 4,
  SIZE_FORMAT           = 4,
  SIZE_SUB_CHUNK1_ID    = 4,
  SIZE_SUB_CHUNK1_SIZE  = 4,
  SIZE_AUDIO_FORMAT     = 2,
  SIZE_NBR_CHANNELS     = 2,
  SIZE_SAMPLE_RATE      = 4,
  SIZE_BYTE_RATE        = 4,
  SIZE_BLOCK_ALIGN      = 2,
  SIZE_BITS_PER_SAMPLE  = 2,
  SIZE_SUB_CHUNK2_ID    = 4,
  SIZE_SUB_CHUNK2_SIZE  = 4,
};

struct wave_format {

  char *chunk_id;
  uint32_t chunk_size;
  char *format;
  char *sub_chunk1_id;
  uint32_t sub_chunk1_size;
  uint16_t audio_format;
  uint16_t nbr_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  char *sub_chunk2_id;
  uint32_t sub_chunk2_size;

};

/*
 *
 */
int parse_wave(char *wavefile, struct wave_format *wf);


#endif /* end of __PARSE_H__ */
