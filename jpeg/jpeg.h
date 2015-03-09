#ifndef JPEG_ENCDEC_LIB
#define JPEG_ENCDEC_LIB

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <jpeglib.h>
#include <magic.h>

bool jpeg_check_file(const char * filename);
void jpeg_decode(const char * filename, int & height, int & width, int & components, unsigned char ** & matrix);
void jpeg_encode(const char * filename, int height, int width, int components, unsigned char ** matrix);
void jpeg_encode_grayscale(const char * filename, int height, int width, unsigned char ** matrix);

#endif