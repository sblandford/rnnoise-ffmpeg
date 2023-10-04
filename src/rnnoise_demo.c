/* Copyright (c) 2018 Gregor Richards
 * Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include "rnnoise.h"

#define FRAME_SIZE 480

#define MAX_PATH_LENGTH 1000
#define COMMAND_LENGTH 400

int main(int argc, char **argv) {
  int i;
  int first = 1;
  float x[FRAME_SIZE];
  FILE *f1, *fout;
  char incommand[(MAX_PATH_LENGTH * 2) + COMMAND_LENGTH], outcommand[(MAX_PATH_LENGTH * 2) + COMMAND_LENGTH];
  DenoiseState *st;
  st = rnnoise_create(NULL);
  /* f1 = fopen(argv[1], "rb");
  fout = fopen(argv[2], "wb"); */
  
  
  if ((argc < 3) || (argc >5)) {
    fprintf(stderr, "usage: %s <noisy speech> <output denoised> [<ffmpeg codec parameters>] [<ffmpeg input filter>]\n", argv[0]);
    return 1;
  }
  if (strlen(argv[1]) > MAX_PATH_LENGTH) {
    fprintf(stderr, "Length of input file path greater than maxiumum %d chars>\n",MAX_PATH_LENGTH);
    return 1;
  }
  if (strlen(argv[2]) > MAX_PATH_LENGTH) {
    fprintf(stderr, "Length of output file path greater than maxiumum %d chars>\n",MAX_PATH_LENGTH);
    return 1;
  }
  if (argc >= 4)  {
    if (strlen(argv[3]) > MAX_PATH_LENGTH) {
        printf("Found output codec\n");
        fprintf(stderr, "Length of output codec greater than maxiumum %d chars>\n",MAX_PATH_LENGTH);
        return 1;
    }
    sprintf(outcommand, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - %s '%s'", argv[3], argv[2]);
  } else {
      sprintf(outcommand, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - '%s'", argv[2]);
  }
  if (argc == 5) {
    if (strlen(argv[4]) > MAX_PATH_LENGTH) {
        printf("Found input filter\n");
        fprintf(stderr, "Length of input filter greater than maxiumum %d chars>\n",MAX_PATH_LENGTH);
        return 1;
    }
    sprintf(incommand, "ffmpeg -i '%s' -filter:a %s -f s16le -ar 44100 -ac 1 -", argv[1], argv[4]);
  } else {
    sprintf(incommand, "ffmpeg -i '%s' -f s16le -ar 44100 -ac 1 -", argv[1]);
  }
  
  /*sprintf(incommand, "sox '%s' -t raw -r 44.1k -e signed -b 16 -c 1 -", argv[1]);
  sprintf(outcommand, "sox -t raw -r 44.1k -e signed -b 16 -c 1 '%s'", argv[2]);*/
  f1 = popen(incommand, "r");
  fout = popen(outcommand, "w");
  while (1) {
    short tmp[FRAME_SIZE];
    fread(tmp, sizeof(short), FRAME_SIZE, f1);
    if (feof(f1)) break;
    for (i=0;i<FRAME_SIZE;i++) x[i] = tmp[i];
    rnnoise_process_frame(st, x, x);
    for (i=0;i<FRAME_SIZE;i++) tmp[i] = x[i];
    if (!first) fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
    first = 0;
  }
  rnnoise_destroy(st);
  fclose(f1);
  fclose(fout);
  return 0;
}
