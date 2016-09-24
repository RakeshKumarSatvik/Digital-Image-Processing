/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016

	File            : This code is for Non-local means noise removal algorithm.
*/

#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
public:
	Image();
	virtual ~Image();
	void allocate_image();
	void read_image(char *filename);
	void write_image(char *filename);
	void warped_image();
	int bilinear_interpolation(float pPrime, int qPrime, int k);

private:
	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *OutputImagedata;

	int width;
	int height;
	int BytesPerPixel;
};

#endif /* IMAGE_H_ */
