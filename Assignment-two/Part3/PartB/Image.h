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

	typedef enum {
		IMAGE_SHRINKING,
		IMAGE_THINNING,
		IMAGE_SKELETONIZING
	}morphology;

	typedef struct{
		int skeletonize_count;
		int thin_count;
	}result;

	Image();
	virtual ~Image();
	void allocate_image();
	void read_image(char *filename);
	void write_image(char *filename, unsigned char *data, int inp_width, int inp_height);
	void morphological_processing(int count);
	void image_morphology(morphology type, int limit);
	std::vector<std::vector<int> > generate_table(morphology type, int table);
	void print_table(morphology type);
	void hole_filling();
	void image_edge_resolver();
	int image_end_points();
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
	std::vector<std::vector<int> > thinning_first_table;
	std::vector<std::vector<int> > skeletoninzing_first_table;
	std::vector<std::vector<int> > shrinking_thinning_second_table;
	std::vector<std::vector<int> > skeletonize_second_table;
	result compare_output[3];
};

#endif /* IMAGE_H_ */
