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

#define LIST_CHUNK_TYPE      "LIST"
#define LIST_CHUNK_INFO_TYPE "INFO"
#define DATA_CHUNK_TYPE      "DATA"

static FILE *fd = NULL;

static int read_requested_size (void *buf, int size) {

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

  if (read_requested_size(temp, size) < 0) {
    return -1;
  }

  temp[size] = '\0';
  *buf = temp;

  log_info("adress buf %p, value %s", *buf, *buf);
  return SUCESS;
}

static int parse_int8(uint8_t **buf, uint32_t size) {

  uint8_t *temp = (uint8_t *) malloc (size * sizeof(uint8_t));
  if (temp == NULL) {
    log_error("malloc error : parse_int8");
    return -1;
  }

  if (read_requested_size(temp, size) < 0) {
    return -1;
  }

  *buf = temp;

  return SUCESS;
}

static int parse_int16(uint16_t *buf) {

  if (read_requested_size(buf, sizeof(uint16_t)) < 0) {
    return -1;
  }

  return SUCESS;
}

static int parse_int32(uint32_t *buf) {

  if (read_requested_size(buf, sizeof(uint32_t)) < 0) {
    return -1;
  }

  return SUCESS;
}

static void parse_print_list_type_info(struct list_type_info *info) {

  struct list_type_info *temp_info = info;

  while (temp_info != NULL) {
    log_message("    Chunk Type   :  INFO", temp_info->info_id);
    log_message("      Info ID    :  %s", temp_info->info_id);
    log_message("      Info Size  :  %d", temp_info->info_size);
    log_message("      Info Text  :  %s", temp_info->info_text);

    temp_info = temp_info->next;
  }
}

void parse_print_chunk(struct chunk *chunks) {

  struct chunk *temp = chunks;

  while (temp != NULL) {
    log_message("  Subchunk2ID    :  %s", temp->sub_chunk2_id);
    log_message("  Subchunk2Size  :  %d", temp->sub_chunk2_size);

    if (strncasecmp(temp->sub_chunk2_id, LIST_CHUNK_TYPE, 4) == 0) {
        parse_print_list_type_info(temp->info);
    }

    temp = temp->next;
  }
}

void parse_print_wave_header(struct wave_format *wf) {

  log_message(" ");
  log_message("Header Information");
  log_message("==================");
  log_message(" ");
  log_message("  ChunkID        :  %s", wf->chunk_id);
  log_message("  ChunkSize      :  %d", wf->chunk_size);
  log_message("  Format         :  %s", wf->format);
  log_message("  Subchunk1ID    :  %s", wf->sub_chunk1_id);
  log_message("  Subchunk1Size  :  %d", wf->sub_chunk1_size);
  log_message("  AudioFormat    :  %d", wf->audio_format);
  log_message("  NumChannels    :  %d", wf->nbr_channels);
  log_message("  SampleRate     :  %d", wf->sample_rate);
  log_message("  ByteRate       :  %d", wf->byte_rate);
  log_message("  BlockAlign     :  %d", wf->block_align);
  log_message("  BitsPerSample  :  %d", wf->bits_per_sample);

  if (wf->chunks != NULL) {
    parse_print_chunk(wf->chunks);
  }

  log_message(" ");
  
  return;
}

static struct chunk *chunk_allocate() {
  struct chunk *temp;

  temp = (struct chunk *) malloc(sizeof(struct chunk));
  if (temp == NULL) {
    return NULL;
  }

  temp->sub_chunk2_id = NULL;
  temp->sub_chunk2_size = 0;
  temp->next = NULL;

  return temp;
}

static struct list_type_info *chunk_info_allocate() {
  struct list_type_info *temp;

  temp = (struct list_type_info *) malloc(sizeof(struct list_type_info));
  if (temp == NULL) {
    return NULL;
  }

  temp->info_id = NULL;
  temp->info_text = NULL;
  temp->info_size = 0;
  temp->next = NULL;

  return temp;
}

static void chunk_info_finalize(struct list_type_info *info) {

  struct list_type_info *temp;
  struct list_type_info *next_node = info;

  do {
    temp = next_node;
    next_node = temp->next;

    if (temp->info_id != NULL) {
      free(temp->info_id);
    }

    if (temp->info_text != NULL) {
      free(temp->info_text);
    }

    if (temp != NULL) {
      free(temp);
    }

  } while (next_node != NULL);

}

static void chunks_finalize(struct chunk *chunks) {

  struct chunk *temp;
  struct chunk *next_node = chunks;

  do {
    temp = next_node;
    next_node = temp->next;

    if (strncasecmp(temp->sub_chunk2_id, DATA_CHUNK_TYPE, 4) == 0) {
      free(temp->data);
    } else if (strncasecmp(temp->sub_chunk2_id, LIST_CHUNK_TYPE, 4) == 0) {
      if (temp->info != NULL) {
        chunk_info_finalize(temp->info);
      }
    }

    if (temp->sub_chunk2_id != NULL) {
      free(temp->sub_chunk2_id);
    }

    if (temp != NULL) {
      free(temp);
    }

  } while (next_node != NULL);

}

static int parse_list_chunk_info (struct chunk *list_chunk, int chunk_size) {

  char *type_id;
  struct list_type_info *prev_info = NULL;
  struct list_type_info *new_info = NULL;

  if (chunk_size <= 0)
    return ERROR_OTHER;

  /* read if it is LIST Type info or not */
  if(parse_char(&type_id, SIZE_LIST_TYPE_ID) < 0) {
    log_error("Failed to parse LIST Info type");
    return ERROR_READ_CHAR;
  }

  // read the list type, so need to reduce 4 bytes
  chunk_size -= 4;

  if (strncasecmp(type_id, LIST_CHUNK_INFO_TYPE, 4) == 0) {
    log_info("Found LIST chunk type : INFO");

    // free type_id, we donot need anymore
    free(type_id);

    while (chunk_size > 0 ) {

      /* We need to create a new info node */
      new_info = chunk_info_allocate();
      if (new_info == NULL) {
        log_error("Failed allocate chunk info memory : malloc");
        return ERROR_ALLOC;
      }

      if (list_chunk->info == NULL) {
        // Initialize the first info node
        list_chunk->info = new_info;

        // Keep track of next info node to append
        prev_info = new_info;
      } else {
        /* We need to append a new info node */
        prev_info->next = new_info;

        // Keep track of next info node to append
        prev_info = new_info;
      }

      /* read info id */
      if(parse_char(&new_info->info_id, SIZE_LIST_INFO_ID) < 0) {
        log_error("Failed to parse LIST Info ID");
        return ERROR_READ_CHAR;
      }
      chunk_size -= SIZE_LIST_INFO_ID;

      /* read info size */
      if(parse_int32(&new_info->info_size) < 0) {
        log_error("Failed to parse LIST Info text size");
        return ERROR_READ_INT;
      }
      chunk_size -= SIZE_LIST_INFO_TEXT_SIZE;
      log_info("Size of this Info text : %d", new_info->info_size);

      /* read info text */
      if(parse_char(&new_info->info_text, new_info->info_size) < 0) {
        log_error("Failed to parse LIST Info text");
        return ERROR_READ_CHAR;
      }

      chunk_size -= new_info->info_size;

    }
  } else {
    log_warning("Found LIST chunk type : %s, skipping", type_id);
  }


  return SUCESS;
 
}
/*
 *
 */
int parse_wave(char *wavefile, struct wave_format *wf) {

  long file_offset;
  struct chunk *prev_chunk = NULL;
  struct chunk *new_chunk = NULL;

  log_message("Start parsing of wave file %s", wavefile);

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

  file_offset = ftell(fd);
  if (file_offset == -1) {
    log_error("Failed to get current postion");
    goto out;
  }

  log_info("Position in the file, %d", file_offset);
  wf->chunks = NULL;

  while(file_offset < wf->chunk_size) {

    /* We need to create a new node */
    new_chunk = chunk_allocate();
    if (new_chunk == NULL) {
      log_error("Failed allocate chunk memory : malloc");
      goto out;
    }

    if (wf->chunks == NULL) {

      // Initialize the first node
      wf->chunks = new_chunk;

      // Keep track of next node to append
      prev_chunk = new_chunk;

    } else {
      /* We need to append a new node */

      prev_chunk->next = new_chunk;

      // Keep track of next node to append
      prev_chunk = new_chunk;
    }

    /* read SubChunk2Id */
    if(parse_char(&new_chunk->sub_chunk2_id, SIZE_SUB_CHUNK2_ID) < 0) {
      log_error("Invalid SubChunkId2");
      goto out;
    }

    // Increase the offset for byte read for SubChunk2Id
    file_offset += SIZE_SUB_CHUNK2_ID;

    /* read SubChunk2Size */
    if(parse_int32(&new_chunk->sub_chunk2_size) < 0) {
      log_error("Invalid SubChunkSize2");
      goto out;
    }

    // Increase the offset by bytes read for SubChunk2Size
    file_offset += SIZE_SUB_CHUNK2_SIZE;

    if (strncasecmp(new_chunk->sub_chunk2_id, LIST_CHUNK_TYPE, 4) == 0) {
      log_debug("Found LIST chunk type, size %d", new_chunk->sub_chunk2_size);

      // Increase the bytes read for real chunk size
      file_offset += new_chunk->sub_chunk2_size;

      // Initialize to NULL, will be used to get the
      // indication for first node
      new_chunk->info = NULL;

      if (parse_list_chunk_info(new_chunk, new_chunk->sub_chunk2_size) <0) {
        log_error("Failed to parse LIST INFO chunk");
        goto out;
      }

      log_info("Position in the file, %d", file_offset);
      if (fseek(fd, file_offset, SEEK_SET) == -1) {
        log_error("Failed to get current postion");
        goto out;
      }

    } else if (strncasecmp(new_chunk->sub_chunk2_id, DATA_CHUNK_TYPE, 4) == 0) {

      log_debug("Found DATA chunk type");

      if(parse_int8(&new_chunk->data, wf->chunks->sub_chunk2_size) < 0) {
        log_error("Error parsing data chunk, size %d", new_chunk->sub_chunk2_size);
        goto out;
      }

      // Increase the bytes read for real data chunk
      file_offset += new_chunk->sub_chunk2_size;


    } else {
      log_error("Unknown chunk, exit with failure");
      goto out;
    }

  }



  return 0;

out:
  /* close the file descriptor */
  parse_finalize(wf);

  return -1;
}

void parse_finalize(struct wave_format *wf) {
  if (wf->chunk_id != NULL) {
    free(wf->chunk_id);
  }

  if (wf->format != NULL) {
    free(wf->format);
  }

  if (wf->sub_chunk1_id != NULL) {
    free(wf->sub_chunk1_id);
  }

  if (wf->chunks != NULL) {
    chunks_finalize(wf->chunks);
  }

  if (fclose(fd) < 0) {
    log_error("Error closing the file, errno = %d.", errno);
  }

}
