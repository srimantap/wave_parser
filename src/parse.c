/*
 *
 * Copyright (C) 2020  Bits For Bytes <support@bitsforbyte.com>
 *
 * parse.c: parse the provided *.wave file
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#include "parse.h"

static FILE *fd = NULL;

static int read_requested_size (void *buf, int size, int endieness) {

  ssize_t read_bytes;

  if (fd == NULL) {
    return ERROR_INVALID_FD;
  }

  read_bytes = fread(buf, 1, size, fd);

  if (read_bytes != size) {
    log_error("Read error, bytes to be read: %d", size);
    return ERROR_INVALID_SIZE;
  }

  return SUCESS;
}

static int parse_char(char **buf, int size) {

  char *temp = (char *) malloc ((size + 1) * sizeof(char));
  if (temp == NULL) {
    log_error("malloc error");
    return -1;
  }

  if (read_requested_size(temp, size, BIG_ENDIEN) < 0) {
    return -1;
  }

  temp[SIZE_CHUNK_ID] = '\0';
  *buf = temp;

  return SUCESS;
}

static int parse_int16(uint16_t *buf) {

  if (read_requested_size(buf, sizeof(uint16_t), LITTLE_ENDIEN) < 0) {
    return -1;
  }

  return SUCESS;
}

static int parse_int32(uint32_t *buf) {

  if (read_requested_size(buf, sizeof(uint32_t), LITTLE_ENDIEN) < 0) {
    return -1;
  }

  return SUCESS;
}


/*
 *
 */
int parse_wave(char *wavefile, struct wave_format *wf) {

  long file_position;

  log_debug("Start parsing of wave file %s", wavefile);

  /* open the input file provided */
  fd = fopen (wavefile, "r");
  if (fd == NULL) {
    log_error("Unable to open the file : %s, errno = %d .", wavefile, errno);
    return -1;
  }

  /* read chunk id */
  if(parse_char(&wf->chunk_id, SIZE_CHUNK_ID) < 0) {
    log_error("Invalid chunk id");
    goto out;
  }

  /* read chunk size */
  if(parse_int32(&wf->chunk_size) < 0) {
    log_error("Invalid chunk size");
    goto out;
  }

  /* read format */
  if(parse_char(&wf->format, SIZE_FORMAT) < 0) {
    log_error("Invalid format");
    goto out;
  }

  /* read SubChunk1Id */
  if(parse_char(&wf->sub_chunk1_id, SIZE_SUB_CHUNK1_ID) < 0) {
    log_error("Invalid SubChunkId");
    goto out;
  }

  /* read SubChunk1Size */
  if(parse_int32(&wf->sub_chunk1_size) < 0) {
    log_error("Invalid SubChunkSize");
    goto out;
  }

  /* read Audio Format */
  if(parse_int16(&wf->audio_format) < 0) {
    log_error("Invalid audio format");
    goto out;
  }

  /* read NumChannels */
  if(parse_int16(&wf->nbr_channels) < 0) {
    log_error("Invalid NumChannels");
    goto out;
  }

  /* read Sample Rate */
  if(parse_int32(&wf->sample_rate) < 0) {
    log_error("Invalid sample rate");
    goto out;
  }

  /* read Byte Rate */
  if(parse_int32(&wf->byte_rate) < 0) {
    log_error("Invalid byte rate");
    goto out;
  }

  /* read Block Align */
  if(parse_int16(&wf->block_align) < 0) {
    log_error("Invalid block align");
    goto out;
  }

  /* read Bits Per Sample */
  if(parse_int16(&wf->bits_per_sample) < 0) {
    log_error("Invalid sample rate");
    goto out;
  }

  /* read SubChunk2Id */
  if(parse_char(&wf->sub_chunk2_id, SIZE_SUB_CHUNK2_ID) < 0) {
    log_error("Invalid SubChunkId2");
    goto out;
  }

  /* read SubChunk2Size */
  if(parse_int32(&wf->sub_chunk2_size) < 0) {
    log_error("Invalid SubChunkSize2");
    goto out;
  }

  file_position = ftell(fd);
  if (file_position == -1) {
    log_error("Failed to get current postion");
    goto out;
  }

  log_info("Position in the file, %d", file_position);






  if (wf->chunk_id != NULL) {
    free(wf->chunk_id);
  }

  if (wf->format != NULL) {
    free(wf->format);
  }

  if (wf->sub_chunk1_id != NULL) {
    free(wf->sub_chunk1_id);
  }

  if (wf->sub_chunk2_id != NULL) {
    free(wf->sub_chunk2_id);
  }

  if (fclose(fd) < 0) {
    log_error("Error closing the file, errno = %d.", errno);
  }

  return 0;

out:
  /* close the file descriptor */
  if (fclose(fd) < 0) {
    log_error("Error closing the file, errno = %d.", errno);
  }

  if (wf->chunk_id != NULL) {
    free(wf->chunk_id);
  }

  if (wf->format != NULL) {
    free(wf->format);
  }

  if (wf->sub_chunk1_id != NULL) {
    free(wf->sub_chunk1_id);
  }

  if (wf->sub_chunk2_id != NULL) {
    free(wf->sub_chunk2_id);
  }

  return -1;
}
