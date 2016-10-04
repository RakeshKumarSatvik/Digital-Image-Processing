/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016

	File            : This code is for Non-local means noise removal algorithm.
*/
#include <vector>

#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
public:
	Image();
	virtual ~Image();
	void allocate_image();
	void read_image(char *filename);
	void write_image(char *filename, unsigned char *data, int inp_width, int inp_height);
	int bilinear_interpolation(unsigned char *data, float pPrime, int qPrime, int k, int inp_width);
	void error_diffusion();
	void padding_image(int padH, int padW);

	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *PaddedImagedata;

	int width;
	int height;
	int BytesPerPixel;
};

#endif /* IMAGE_H_ */
