#include "jpeg.h"

bool jpeg_check_file(const char * filename){
    const char * type = "jpeg";
    return file_check_type(filename, type);
}

void jpeg_decode(const char * filename, int & height, int & width, int & components, unsigned char ** & matrix){
    int buffer_height = 1;
    int row_stride;
    unsigned char ** image;

    // row_stride and buffer needs to be allocated dynamically
    // 1. Allocaiton and initialization
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // 2. Specify that source of the compressed data
    FILE * infile = fopen(filename, "rb");
    if (infile == NULL) {
        fprintf(stderr, "cant open%s\n", filename);
        exit(1);
    }
    jpeg_stdio_src(&cinfo, infile);
    
    // 3. Obtain image info (width, height etc.)
    jpeg_read_header(&cinfo, TRUE);
    
    // 4. Parameters for decompression
    // 5. jpeg_start_decompress
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    components = cinfo.output_components;
    row_stride = cinfo.output_width * cinfo.output_components;
    image = new unsigned char * [cinfo.output_height];

    // cinfo now contains the following information
    // output_width                 # image width and height, as scaled
    // output_height
    // out_color_components         # of color components in out_color_space
    // output_components            # of color components returned per pixel
    // colormap                     # the selected colormap, if any
    // actual_number_of_colors      # number of entries in colormap

    // 6. while(scanlines remaining, keep scanline)
    for (int i = 0 ; cinfo.output_scanline < cinfo.output_height ; i++) {
        unsigned char * buffer = new unsigned char[row_stride];
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        image[i] = buffer;
    }

    // 7. Jpeg finish decompress
    jpeg_finish_decompress(&cinfo);

    // 8. Final step, destroy decompression object and release the object
    jpeg_destroy_decompress(&cinfo);

    // set the return values
    matrix = image;
}

void jpeg_encode(const char * filename, int height, int width, int components, unsigned char ** image) {
    // 1. Allocate and initialize memory location
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // 2. Specify a desintation for the output file
    FILE * outfile;
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    // 3. Set parameters for image compression
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = components;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);

    // 4. Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // 5. While line remains to be written, write lines
    JSAMPROW row_pointer[1];    /* pointer to a single row */
    int row_stride;             /* physical row width in buffer */

    row_stride = width * components;   /* JSAMPLEs per row in image_buffer */

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = image[cinfo.next_scanline];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // 6. Finish compress
    jpeg_finish_compress(&cinfo);

    // 7. Release the object
    jpeg_destroy_compress(&cinfo);
}

void jpeg_encode_grayscale(const char * filename, int height, int width, unsigned char ** image){
    // 1. Allocate and initialize memory location
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // 2. Specify a desintation for the output file
    FILE * outfile;
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    // 3. Set parameters for image compression
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);

    // 4. Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // 5. While line remains to be written, write lines
    int row_stride;             /* physical row width in buffer */
    row_stride = width * 3;   /* JSAMPLEs per row in image_buffer */
    JSAMPROW row_pointer[1];    /* pointer to a single row */
    row_pointer[0] = new unsigned char[row_stride];
    
    // flush();
    while (cinfo.next_scanline < cinfo.image_height) {
        // printf("%d\n", cinfo.nex);
        for(int i = 0 ; i < width ; i++) {
            row_pointer[0][3 * i] = image[cinfo.next_scanline][i];
            row_pointer[0][3 * i + 1] = image[cinfo.next_scanline][i];
            row_pointer[0][3 * i + 2] = image[cinfo.next_scanline][i];
        }

        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // 6. Finish compress
    jpeg_finish_compress(&cinfo);

    // 7. Release the object
    jpeg_destroy_compress(&cinfo);
}