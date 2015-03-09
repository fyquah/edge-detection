#ifndef PNG_H
#define PNG_H

#include <stdio.h>
#include <vector>
#include <magic.h>
#include "../lodepng/lodepng.h"
#include "../file/file.h"

bool png_check_file(const char * filename);
void png_decode(const char * filename, int & height, int & width, int & components, unsigned char ** & matrix);
void png_encode();
void png_encode_grayscale(const char * filename, int height, int width, int components, unsigned char ** & matrix);

#endif