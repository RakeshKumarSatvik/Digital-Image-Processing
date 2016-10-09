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
	void homographical_processing();
	void image_grayscale();
	void padding_image(unsigned char *data);
	void image_binarization();
	void image_shrinking();
	void generate_table();
	void print_table();
	void hole_filling();
	void image_smoothening();

	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *OutputImagedata;
	unsigned char *PaddedImagedata;
    unsigned char *BinaryImagedata;
    unsigned char *TempImagedata;
    unsigned char *HitorMissImagedata;

	int width;
	int height;
	int BytesPerPixel;
	std::vector<std::vector<int> > shrinking_first_table;
	std::vector<std::vector<int> > shrinking_second_table;
	std::vector<std::vector<int> > skeletonize_second_table;
};

#endif /* IMAGE_H_ */
