#include "lodepng/lodepng.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <utility>
#include <vector>
typedef unsigned char byte; 
using std::cin;
using std::cout;
using std::endl;

namespace MASK {
    const int x[3][3] = {
        { -10, 0, 10 },
        { -3, 0, 3 },
        { -10, 0, 10 }
    };
    const int y[3][3] = {
        { -10, -3, -10 },
        { 0, 0, 0 },
        { 10, 3, 10 }
    };
}

void encode_grayscale(const std::vector< std::vector<byte> > & output, unsigned width, unsigned height, const char * filename);
void find_edge(const char* input_filename, const char * output_filename);
std::pair<int, int> convolute(int y, int x, const std::vector<std::vector<byte> > & pixels);
inline int magnitude(byte gx, byte gy);


int main(int argc, char *argv[])
{
    assert(argc >= 3 && "Expected input file name and output file name as CLI argument!!");
    const char* input_filename = argv[1];
    const char* output_filename = argv[2];
    find_edge(input_filename, output_filename);
}

void encode_grayscale(const std::vector< std::vector<byte> > & output, unsigned width, unsigned height, const char * filename) {
    std::vector<byte> image;
    for(int y = 0 ; y < height ; y++) {
        for(int x = 0 ; x < width ; x++) {
            for(int i = 0 ; i < 4 ; i++) {
                image.push_back(output[y][x]);
            }
        }
    }
    unsigned error = lodepng::encode(filename, image, width, height);
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void find_edge(const char* input_filename, const char * output_filename) {
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;
    std::vector< std::vector<byte> > red;
    std::vector< std::vector<byte> > green;
    std::vector< std::vector<byte> > blue;
    std::vector< std::vector<byte> > alpha;
    std::vector< std::vector<int> > delta_x_red;
    std::vector< std::vector<int> > delta_x_green;
    std::vector< std::vector<int> > delta_x_blue;
    std::vector< std::vector<int> > delta_x_alpha;
    std::vector< std::vector<int> > delta_y_red;
    std::vector< std::vector<int> > delta_y_green;
    std::vector< std::vector<int> > delta_y_blue;
    std::vector< std::vector<int> > delta_y_alpha;
    std::vector< std::vector<byte> > output;
    //decode
    unsigned error = lodepng::decode(image, width, height, input_filename);
    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    red.assign(height, std::vector<byte>(width));
    green.assign(height, std::vector<byte>(width));
    blue.assign(height, std::vector<byte>(width));
    alpha.assign(height, std::vector<byte>(width));
    delta_x_red.assign(height, std::vector<int>(width));
    delta_x_green.assign(height, std::vector<int>(width));
    delta_x_blue.assign(height, std::vector<int>(width));
    delta_x_alpha.assign(height, std::vector<int>(width));
    delta_y_red.assign(height, std::vector<int>(width));
    delta_y_green.assign(height, std::vector<int>(width));
    delta_y_blue.assign(height, std::vector<int>(width));
    delta_y_alpha.assign(height, std::vector<int>(width));
    output.assign(height, std::vector<byte>(width, 0));

    for(int i = 0 ; i < image.size() ; ) {
        int y = (i / 4) / width;
        int x = (i / 4) % width;
        red[y][x] = image[i++];
        green[y][x] = image[i++];
        blue[y][x] = image[i++];
        alpha[y][x] = image[i++];
    }

    for(int y = 1 ; y < height - 1 ; y++) {
        for(int x = 1 ; x < width - 1 ; x++) {
            // evaluate gx on every color
            std::pair<int, int> res = convolute(y, x, red);
            delta_y_red[y][x] = res.first;
            delta_x_red[y][x] = res.second;
            // cout << y << ',' << x << endl;

            // evaluate gy on every color
            res = convolute(y, x, green);
            delta_y_green[y][x] = res.first;
            delta_x_green[y][x] = res.second;

            res = convolute(y, x, blue);
            delta_y_blue[y][x] = res.first;
            delta_x_blue[y][x] = res.second;

            res = convolute(y, x, alpha);
            delta_y_alpha[y][x] = res.first;
            delta_x_alpha[y][x] = res.second;
        }
    }

    // color-ed edges
    // image.clear();
    // for(int y = 0 ; y < height ; y++) {
    //     for(int x = 0 ; x < width; x++) {
    //         image.push_back(magnitude(delta_y_red[y][x], delta_x_red[y][x]));
    //         image.push_back(magnitude(delta_y_green[y][x], delta_x_green[y][x]));
    //         image.push_back(magnitude(delta_y_blue[y][x], delta_x_blue[y][x]));
    //         image.push_back(255);
    //     }
    // }
    // error = lodepng::encode("colored.png", image, width, height);
    // if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

    // grayscale edges
    for(int y = 1 ; y < height - 1; y++) {
        for(int x = 1 ; x < width - 1 ; x++) {
            double gy, gx;

            gx = delta_x_red[y][x] + delta_x_blue[y][x] + delta_x_green[y][x] + delta_x_alpha[y][x];
            gx /= 3;
            gy = delta_y_red[y][x] + delta_y_blue[y][x] + delta_y_green[y][x] + delta_y_alpha[y][x];
            gy /= 3;
            gy = (gy > 255 ? 255 : gy);
            gx = (gx > 255 ? 255 : gx);
            
            output[y][x] = sqrt(gx * gx + gy * gy);    
        }
    }
    encode_grayscale(output, width, height, output_filename);
}

std::pair<int, int> convolute(int y, int x, const std::vector< std::vector<byte> > & pixels){
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
