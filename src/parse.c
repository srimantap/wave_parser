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
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "log.h"
#include "parse.h"

static int fd = -1;

static int read_requested_size (void *buf, int size) {

  ssize_t read_bytes;

  if (fd < 0) {
    return ERROR_INVALID_FD;
  }

  read_bytes = read(fd, buf, size);

  if ((read_bytes == -1) || (read_bytes < size)) {
    return ERROR_INVALID_SIZE;
  }

  return SUCESS;
}

static int parse_chunk_id(void *buf) {

  if (read_requested_size(buf, SIZE_CHUNK_ID) < 0) {
    return -1;
  }

  log_debug("Read complete.");
  return 0;
}

/*
 *
 */
int parse_wave(char *wavefile) {

  struct wave_format wf;

  log_warning("Start parsing of wave file %s", wavefile);

  /* open the input file provided */
  fd = open (wavefile, O_RDONLY);
  if (fd < 0) {
    log_error("Unable to open the file : %s, errno = %d .", wavefile, errno);
    return -1;
  }

  /* read chunk id */
  if(parse_chunk_id(&wf.chunk_id) < 0) {
    log_error("Invalid chunk id");
    goto out;
  }

  return 0;

out:
  /* close the file descriptor */
  if (close(fd) < 0) {
    log_error("Error closing the file, errno = %d.", errno);
  }

  return -1;
}
