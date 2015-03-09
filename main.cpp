#include "png/png.h"
#include "jpeg/jpeg.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <utility>
#include <vector>
typedef unsigned char byte; 
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

namespace MASK {
    const int x[3][3] = {
        { -2, 0, 2 },
        { -1, 0, 1 },
        { -2, 0, 2 }
    };
    const int y[3][3] = {
        { -2, -1, -2 },
        { 0, 0, 0 },
        { 2, 1, 2 }
    };
}

const int FILE_TYPE_PNG = 1;
const int FILE_TYPE_JPEG = 2;
const int FILE_TYPE_ERR = -1;

int get_file_type(const char * filename);
void binarify_edge(unsigned char ** grayscale, const int height, const int width, const int threshold);
void evaluate_grayscale(std::vector <int **> & image, int height, int width, unsigned char ** & grayscale);
void find_delta(unsigned char ** image, int height, int width, int ** & gy, int ** & gx);
std::pair<int, int> convolute(int y, int x, unsigned char ** image);
inline int magnitude(byte gx, byte gy);

int main(int argc, char *argv[])
{
    assert(argc >= 3 && "Expected input file name and output file name as CLI argument!!");
    int height, width, components_count;
    unsigned char ** image;
    unsigned char ** grayscale;
    std::vector< unsigned char ** > image_components;
    std::vector< int ** > vec_gy;
    std::vector< int ** > vec_gx;
    std::vector< int ** > vec_g;

    if(jpeg_check_file(argv[1])){
        jpeg_decode(argv[1], height, width, components_count, image);
    } else if(png_check_file(argv[1])){
        png_decode(argv[1], height, width, components_count, image);
    } else {
        cerr << "Unrecognized file type!" << endl;
        cerr << "Terminating now!" << endl;
        return 1;
    }

    // create vector of unsigned chars
    for(int i = 0 ; i < components_count ; i++) {
        unsigned char ** ptr = new unsigned char * [height];
        for(int i =0 ; i < width ; i++) {
            ptr[i] = new unsigned char[width];
        }
        image_components.push_back(ptr);
    }

    // extract the components
    for(int y = 0 ; y < height ; y++) {
        for(int x = 0 ; x < width ; x++) {
            for (int i = 0 ; i < components_count ; i++) {
                image_components[i][y][x] = image[y][components_count * x + i];
            }
        }
    }

    for(int i = 0 ; i < height ; i++) {
        delete[] image[i];
    }
    delete[] image;
    image = NULL;
    // N.B. : image is now undefined!

    // Now we get Gx and Gy for every cell (except edge cases)
    for(int i = 0 ; i < components_count ; i++) {
        int ** gy;
        int ** gx;
        find_delta(image_components[i], height, width, gy, gx);
        vec_gx.push_back(gx);
        vec_gy.push_back(gy);
    }

    for(int c = 0 ; c < components_count ; c++) {
        int ** ptr = new int *[height];
        
        for(int y = 0 ; y < height ; y++) {
            ptr[y] = new int[width];

            for(int x = 0 ; x < width ; x++) {
                ptr[y][x] = magnitude(vec_gy[c][y][x], vec_gx[c][y][x]);
            }
        }

        vec_g.push_back(ptr);
    }
    // vec_g now contains the magnitude of all deltas
    evaluate_grayscale(vec_g, height, width, grayscale);
    // binarify_edge(grayscale, height, width, 250);

    // encode into intended file format
    int output_file_type = get_file_type(argv[2]);
    switch(output_file_type) {
        case FILE_TYPE_PNG:
        
        break;
        case FILE_TYPE_JPEG:
        jpeg_encode_grayscale(argv[2], height, width, grayscale);
        break;

        default:
        case FILE_TYPE_ERR:
        cout << "Unkown output file type! Using default of jpeg" << endl;
        jpeg_encode_grayscale(argv[2], height, width, grayscale);
        break;
    }
    

    // Free the memory space!
    for(int i = 0 ; i < height ; i++) {
        delete[] grayscale[i];
    }
    delete[] grayscale;

    for(int c = 0 ; c < image_components.size() ; c++) {
        for(int i = 0 ; i < height ; i++) {
            delete[] vec_gy[c][i];
            delete[] vec_gx[c][i];
            delete[] image_components[c][i];
        }

        delete[] vec_gy[c];
        delete[] vec_gx[c];
        delete[] image_components[c];
    }
}

void binarify_edge(unsigned char ** grayscale, const int height, const int width, const int threshold) {
    for(int y = 0 ; y < height ; y++) {
        for(int x = 0 ; x < width ; x++) {
            grayscale[y][x] = (grayscale[y][x] > threshold ? 255 : 0);
        }
    }
}

void evaluate_grayscale(std::vector <int **> & image, int height, int width, unsigned char ** & grayscale) {
    grayscale = new unsigned char * [height];
    for(int y = 0 ; y < height ; y++) {
        grayscale[y] = new unsigned char[width];
        for(int x = 0 ; x < width ; x++) {
            int a = 0;
            for(int i = 0 ; i < image.size() ; i++) {
                a += image[i][y][x];
            }
            grayscale[y][x] = a / image.size();
        }
    }
}

void find_delta(unsigned char ** image, int height, int width, int ** & gy, int ** & gx) {
    gy = new int * [height];
    gx = new int * [height];

    for(int y = 0 ; y < height  ; y++) {
        gy[y] = new int[width];
        gx[y] = new int[width];

        for(int x = 0 ; x < width ; x++) {
            if(y == 0 || x == 0 || y == height - 1 || x == width - 1 ) {
                gy[y][x] = 0;
                gx[y][x] = 0;
            } else {
                std::pair<int, int> res = convolute(y, x, image);
                gy[y][x] = res.first;
                gx[y][x] = res.second;
            }
        }
    }
}

std::pair<int, int> convolute(int y, int x, unsigned char ** pixels){
    // Takes arguments row, column and pixel matrix
    // returns an Integer pair with the results of convolution
    int gx = 0; // important for tmp to be unsigned!!
    gx += MASK::x[0][0] * pixels[y-1][x-1];
    gx += MASK::x[0][1] * pixels[y-1][x];
    gx += MASK::x[0][2] * pixels[y-1][x+1];
    gx += MASK::x[1][0] * pixels[y][x-1];
    gx += MASK::x[1][1] * pixels[y][x];
    gx += MASK::x[1][2] * pixels[y][x-1];
    gx += MASK::x[2][0] * pixels[y+1][x-1];
    gx += MASK::x[2][1] * pixels[y+1][x];
    gx += MASK::x[2][2] * pixels[y+1][x+1];

    int gy = 0; // important for tmp to be unsigned!!
    gy += MASK::y[0][0] * pixels[y-1][x-1];
    gy += MASK::y[0][1] * pixels[y-1][x];
    gy += MASK::y[0][2] * pixels[y-1][x+1];
    gy += MASK::y[1][0] * pixels[y][x-1];
    gy += MASK::y[1][1] * pixels[y][x];
    gy += MASK::y[1][2] * pixels[y][x-1];
    gy += MASK::y[2][0] * pixels[y+1][x-1];
    gy += MASK::y[2][1] * pixels[y+1][x];
    gy += MASK::y[2][2] * pixels[y+1][x+1];

    return std::make_pair((byte) gy, (byte) gx);
}

inline int magnitude(byte gx, byte gy){
    return (byte) sqrt(gx * gx + gy * gy);
}

int get_file_type(const char * filename){
    int i;
    for(i = 0 ; filename[i] ; i++);
    i--;
    if((filename[i-3] == 'j' && filename[i-2] == 'p' && filename[i-1] == 'e' && filename[i] == 'g') || 
        (filename[i-2] == 'j' && filename[i-1] == 'p' && filename[i] == 'g')) {
        return FILE_TYPE_JPEG;
    } else if (filename[i-2] == 'p' && filename[i-1] == 'n' && filename[i] == 'g') {
        return FILE_TYPE_PNG;
    } else {
        return FILE_TYPE_ERR;
    }
}
