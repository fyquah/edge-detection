#include "file.h"

bool file_check_type(const char * filename, const char * type) {
    int i;
    magic_t cookie = magic_open(MAGIC_NONE);
    if( magic_error(cookie)) {
        printf("An error occured:\n");
        printf("%s\n", magic_error(cookie));
        return false;
    }
    magic_load(cookie, NULL);
    const char * des = magic_file(cookie, filename);

    for(i = 0 ; type[i] && des[i] ; i++) {
        if(tolower(des[i]) != tolower(type[i])) {
            magic_close(cookie);
            return false;
        }
    }
    magic_close(cookie);
    return i == strlen(type);
}