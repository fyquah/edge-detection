#ifndef JPEG_ENCDEC_LIB
#define JPEG_ENCDEC_LIB

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

void decode_jpeg(const char * filename, int & height, int & width, int & components, unsigned char ** & matrix);
void encode_jpeg(const char * filename, int height, int width, int components, unsigned char ** matrix);

#endif