/*
    Name            : Rakesh Kumar Satvik
    USC ID          : 8772-6992-51
    USC email       : satvik@usc.edu
    Submission date : 11/06/2016

    File            : This code is for Texture Analysis.
*/

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
public:
	Image();
	virtual ~Image();
	void allocate_image();
	void read_image(char *filename);
	void write_image(char *filename, unsigned char *data, int inp_width, int inp_height);
	void texture_analysis();
	std::vector<int> calculate_coefficients(int first, int second);
	void apply_mask(std::vector<int> coefficients);
	void print_filters(std::vector<int> coefficients);
	void padding_image(int padH, int padW);
	double energy_calculator();
	double calculate_mean();
	void subtract_mean(double mean);
	void normalize_dump(std::vector<double> energy);
	void print_pca_values();
	void pca_analysis();
	void kmeans_analysis();

	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *PaddedImagedata;
	unsigned char *OutputImagedata;

	int width;
	int height;
	int BytesPerPixel;
	double **coefficients;
	int image_count;
	cv::Mat data;
};

#endif /* IMAGE_H_ */
