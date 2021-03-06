/*
 * 
 * Copyright (C) 2020  Bits For Bytes <support@bitsforbyte.com>
 *
 * main.c: main point for the wave parser
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
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "log.h"
#include "parse.h"

#define HELP_STRING "\n"\
"Usage: wave_parser [OPTIONS]\n"\
"Parse a WAVE file\n"\
"FILE is the input file to parse\n"\
"\n"\
"Options:\n"\
"    -f, --file=INFIL         input to read from INFILE\n"\
"    -h, --help               This help string\n"\
"    -o, --outfile=OUTFILE    write output to OUTFILE\n"\
"                             default is set to standard output\n"\
"    -v, --verbose            set the log level for debugging\n"\
"                             values:\n"\
"                                 MESSAGE = 1,\n"\
"                                 ERROR   = 2,\n"\
"                                 WARNING = 3, default\n"\
"                                 DEBUG   = 4\n"\
"                                 INFO    = 5\n"


int main(int argc, char **argv) {

  char *filename = NULL;
  char *verbose = NULL;
  int c;
  struct wave_format wave_fmt;

  opterr = 0;
  
  /* FIXME: currently the parsing works with short option only.
   *        Plan is to fix it with long option needs to be
   *        taken care.
   */
  /* Parse program options */
  while ((c = getopt (argc, argv, "hv:f:o:")) != -1)
    switch (c)
    {
      case 'f':
        filename = optarg;
        log_info("Input filename = %s", filename);
        break;
      case 'v':
        verbose = optarg;

        if (log_set_level(verbose)) {
          log_error("Wrong verbose level provided");
          exit(1);
        }

        break;
      case 'h':
        printf("%s", HELP_STRING);
        exit(0);
      case '?':
        log_error("Invalid option. Use '-h' to see the help");
        exit(1);
      default:
        abort ();
    }

  /* Input file validation */
  if (filename == NULL) {
    log_error("Input file is missing. Use '-h' option for command option");
    exit(1);
  }

  /* parse the wave file */
  if (parse_wave(filename, &wave_fmt) != 0) {
    log_error("Error while parsing the file.");
    exit(1);
  }

  /* print the wave header */
  parse_print_wave_header(&wave_fmt);

  /* finalize */
  parse_finalize(&wave_fmt);

  exit(0);
}
