#include<stdio.h>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>

//call jpeg_finish_compress/decompress and peg_destroy_compress/decompress
#define JPEG_OPEN(name, cinfo) struct jpeg_error_mgr jerr;\
				cinfo.err=jpeg_std_error(&jerr);\
				jpeg_create_ ## name (&cinfo);

//call jpeg_finish_compress/decompress and peg_destroy_compress/decompress
#define JPEG_CLOSE(name, cinfo) jpeg_finish_ ## name (&cinfo);\
				jpeg_destroy_ ## name (&cinfo);

static void convertAndWrite(struct jpeg_decompress_struct *in_cinfo, int quality, char * filename) {
	//compress info
	struct jpeg_compress_struct cinfo;
	FILE * outfile;		/* target file */
	int row_stride;		/* physical row width in image buffer */
	JSAMPARRAY buffer;		/* Output row buffer */

	//open compress instance
	JPEG_OPEN(compress, cinfo);

	//open write file
	if ((outfile = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return;
	}

	//write to dest file
	jpeg_stdio_dest(&cinfo, outfile);

	//set information
	cinfo.image_width = in_cinfo->output_width;
	cinfo.image_height = in_cinfo->output_height;
	cinfo.input_components = in_cinfo->output_components;
	cinfo.data_precision=in_cinfo->data_precision;
	cinfo.in_color_space = in_cinfo->out_color_space;
	//have to set default first
	jpeg_set_defaults(&cinfo);

	//set quality and color space
	jpeg_set_quality(&cinfo, quality, TRUE);
	//set YCbCr 2x1x1
	cinfo.comp_info[0].v_samp_factor=1;
	cinfo.comp_info[1].v_samp_factor=1;
	cinfo.comp_info[2].v_samp_factor=1;


	jpeg_start_compress(&cinfo, TRUE);

	row_stride = in_cinfo->output_width * in_cinfo->output_components;
	buffer = (*in_cinfo->mem->alloc_sarray)
		((j_common_ptr) in_cinfo, JPOOL_IMAGE, row_stride, 1);

	//write
	while (in_cinfo->output_scanline < in_cinfo->output_height) {
		//read, this data is related to out_color_space
		(void) jpeg_read_scanlines(in_cinfo, buffer, 1);

		//write, have to convert from RGB to YCbCr
		(void) jpeg_write_scanlines(&cinfo, buffer, 1);
	}

	//close
	JPEG_CLOSE(compress, cinfo);
	fclose(outfile);
}

static int convert2YCbCr422 (char * infilename, char * outfilename, int quality) {
	//decompress jpeg
	struct jpeg_decompress_struct cinfo;
	FILE * infile;		/* source file */

	//open file
	if ((infile = fopen(infilename, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", infilename);
		return 0;
	}

	//open decompress instance
	JPEG_OPEN(decompress, cinfo);

	//load from src file
	jpeg_stdio_src(&cinfo, infile);

	//3. read header
	(void) jpeg_read_header(&cinfo, TRUE);

	//4. start decompress!
	// we can load data by using peg_read_scanlines(&cinfo, buffer, 1);
	(void) jpeg_start_decompress(&cinfo);
 
	//convert
	convertAndWrite(&cinfo, quality, outfilename);

	//exit to use decompress
	JPEG_CLOSE(decompress, cinfo);
	fclose(infile);

	return 1;
}

int main(int argc, char*argv[]) {
	if (argc != 4) {
		printf("Usage: %s <infile> <outfile> <quality (low:0-100:high)>\n", argv[0]);
		return 1;
	}

	char *infile=argv[1];
	char *outfile=argv[2];
	int quality=atoi(argv[3]);

 	int ret = convert2YCbCr422(infile, outfile, quality);
	if(!ret) {
		fprintf(stderr, "Failed to convert image\n");
		return 1;
	}
	printf("Success to convert image %s\n", outfile);
	return 0;
}
