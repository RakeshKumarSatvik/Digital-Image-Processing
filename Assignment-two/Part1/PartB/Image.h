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
	void read_image(char *filename, char *filename2, char *filename3);
	void write_image(char *filename, unsigned char *data, int inp_width, int inp_height);
	int bilinear_interpolation(unsigned char *data, float pPrime, int qPrime, int k, int inp_width);
	void image_rotation(int start_x, int start_y, float angle);
	std::vector<std::pair<int,int> > get_edge_coordinates(int start_x, int start_y, unsigned char *data, int width, int height, int second);
	void fill_image(unsigned char* data);
	void crop_image();
	void resize_image(int EnhX, int EnhY, int SizeX, int SizeY);

//private:
	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *SecondImagedata;
	unsigned char *ThirdImagedata;
	unsigned char *OutputImagedata;
	unsigned char *RotatedImagedata;
	unsigned char *ResizeImagedata;
	unsigned char *CroppedImagedata;

	int width;
	int height;
	int BytesPerPixel;
	int hypotenuse;
	int prev_x, prev_y;
};

#endif /* IMAGE_H_ */
