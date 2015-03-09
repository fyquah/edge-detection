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

void png_encode_grayscale(const char * filename, int height, int width, unsigned char ** matrix) {
    std::vector<unsigned char> image;
    for(int y = 0 ; y < height ; y++) {
        for(int x = 0 ; x < width ; x++) {
            for(int i = 0 ; i < PNG_NUMBER_COMPONENTS ;i++) {
                image.push_back(matrix[y][x]);
            }
        }
    }
    unsigned error = lodepng::encode(filename, image, width, height);
    if(error) {
        fprintf(stderr, "Encoder error: %d : %s", error, lodepng_error_text(error));
    } 
}