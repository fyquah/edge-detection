#ifndef FILE_H
#define FILE_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <magic.h>

bool file_check_type(const char * filename, const char * ext);

#endif