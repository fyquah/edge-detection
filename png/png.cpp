#include "png.h"

bool png_check_file(const char * filename) {
    const char * type = "png";
    return file_check_type(filename, type);
}

void png_decode(const char * filename, int & height, int & width, int & components, unsigned char ** & matrix){
    components = 4; // png always have 4 components, I THINK
    std::vector<unsigned char> image;
    unsigned a,b, row_stride;
    a = height;
    b = width;
    lodepng::decode(image, b, a, filename);
    width = b;
    height = a;
    row_stride = width * components;

    matrix = new unsigned char*[height];
    for(int y = 0 ; y < height ; y++) {
        matrix[y] = new unsigned char[row_stride];
        for(int x = 0 ; x < row_stride ; x++) {
            matrix[y][x] = image[y*row_stride + x];
        }
    }
}

void png_encode(){
    // do nithing, for now
}

void png_encode_grayscale(const char * filename, int height, int width, int components, unsigned char ** matrix) {
    
}