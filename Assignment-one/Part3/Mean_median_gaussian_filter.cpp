/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016
	
	File            : This code is for mean, median and gaussian filter noise removal algorithm.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <math.h>
#include <algorithm>

using namespace std;

#define MEDIAN
#define WINDOW_SIZE 5

/*
	Function: CalcMedian
	input  	: pixel_values = the vector of pixel values whose median is to be determined
	output	: returns the median pixel vaue
	
	Description: This function is used to determine the median of the pixels in the vector as input
*/
double CalcMedian(vector<int> pixel_values) {
	double median;
	size_t size = pixel_values.size();
	sort(pixel_values.begin(), pixel_values.end());

	if(size % 2 == 0) {
		median = (pixel_values.at(size / 2 - 1) + pixel_values.at(size / 2)) / 2;
	} else {
		median = pixel_values.at(size/2);
	}
	return median;
}

/*
	Function : Mean Filtering
	input	 : Imagedata		= input image
			   PaddedImagedata	= padded image data
			   padW				= padding width
			   padH				= padding height
			   width			= width of the image
			   height			= height of the image
			   BytesPerPixel	= No of channels in the input image
	output	 : void funciton, no output
	
	Description: The purpose of this function is to take the image data and apply mean filter to it
				 depending on the window size and populate the output buffer with the mean values.
*/
void mean_filtering(unsigned char* Imagedata, unsigned char *PaddedImagedata, int padW, int padH, int width, int height, int BytesPerPixel) {
	int sum = 0;

	// Loop through each channel of the given input image
	for(int k = 0; k < BytesPerPixel; k++) {
		// Loop through the height of the image
		for(int i = 0; i < height; i++) {
			// Loop through the width of the image
			for(int j = 0; j < width; j++) {
				sum = 0;
				//Calculating the mean value for the given window size
				for(int m = 0; m < WINDOW_SIZE; m++) {
					for(int n = 0; n < WINDOW_SIZE; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						sum += value;
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = sum / (WINDOW_SIZE * WINDOW_SIZE);
			}
		}
	}
}

/*
	Function : Median Filtering
	input	 : Imagedata		= input image
			   PaddedImagedata	= padded image data
			   padW				= padding width
			   padH				= padding height
			   width			= width of the image
			   height			= height of the image
			   BytesPerPixel	= No of channels in the input image
	output	 : void funciton, no output
	
	Description: The purpose of this function is to take the image data and apply median filter to it
				 depending on the window size and populate the output buffer with the median values.
*/
void median_filtering(unsigned char* Imagedata, unsigned char *PaddedImagedata, int padW, int padH, int width, int height, int BytesPerPixel) {
	vector<int> image_pixels;

	// Loop through each channel of the given input image
	for(int k = 0; k < BytesPerPixel; k++) {
		// Loop through the height of the image
		for(int i = 0; i < height; i++) {
			// Loop through the width of the image
			for(int j = 0; j < width; j++) {
				image_pixels.clear();
				//Calculating the median value depending on the window size
				for(int m = 0; m < WINDOW_SIZE; m++) {
					for(int n = 0; n < WINDOW_SIZE; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						image_pixels.push_back(value);
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = int(CalcMedian(image_pixels));
			}
		}
	}
}

/*
	Function Calculate_gaussian_values:
	input  : count  = Window size of the kernel for gaussian filter
	output : vector = the values generated with the  
	
	Description: This function takes the input of the kernel size needed to be 
	generated and gives back the vecotr of values.
*/
vector<float> gaussian_filter_values(int count) {
	vector<float> values;
	float temp, sigma = 1;
	int shift = (WINDOW_SIZE / 2) + 1;

	for(int i = 1; i <= count; i++) {
		for(int j = 1; j <= count; j++) {
			temp = exp (- (float(((shift - i) * (shift - i)) + ((shift - j) * (shift - j))) / (2.0 * sigma * sigma)));
			values.push_back(temp);
		}
	}
	return values;
}

/*
	Function : Gaussian Filtering
	input	 : Imagedata		= input image
			   PaddedImagedata	= padded image data
			   padW				= padding width
			   padH				= padding height
			   width			= width of the image
			   height			= height of the image
			   BytesPerPixel	= No of channels in the input image
	output	 : void funciton, no output
	
	Description: The purpose of this function is to take the image data and apply Gaussian filter to it
				 depending on the window size and populate the output buffer with the values generated.
*/
void gaussian_filtering(unsigned char* Imagedata, unsigned char *PaddedImagedata, int padW, int padH, int width, int height, int BytesPerPixel) {
	int sum = 0;
	vector<float> values = gaussian_filter_values(WINDOW_SIZE);
	
	// Loop through each channel of the given input image
	for(int k = 0; k < BytesPerPixel; k++) {
		// Loop through the height of the image
		for(int i = 0; i < height; i++) {
			// Loop through the width of the image
			for(int j = 0; j < width; j++) {
				sum = 0;
				//Calculating the mean value
				for(int m = 0; m < WINDOW_SIZE; m++) {
					for(int n = 0; n < WINDOW_SIZE; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						value = value * (values.at(n + m * WINDOW_SIZE));
						sum += value;
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = sum / (WINDOW_SIZE + 1);
			}
		}
	}
}

/*
	Function : Padding image
	input	 : Imagedata		= input image
			   PaddedImagedata	= padded image data
			   padW				= padding width
			   padH				= padding height
			   width			= width of the image
			   height			= height of the image
			   BytesPerPixel	= No of channels in the input image
	output	 : void funciton, no output
	
	Description: The purpose of this function is to take the image data and depending on the padding value, pad the
				top, bottom, right and left sides of the image.
*/
void padding_image(unsigned char *Imagedata, unsigned char *PaddedImagedata, int height, int width, int padH, int padW, int BytesPerPixel){
	for(int i = 0; i < height; i++) {
		//Left padded rows
		memcpy(PaddedImagedata + (i + padH) * (width + 2 * padW) * BytesPerPixel,
				Imagedata + (i * width) * BytesPerPixel,
				padH * BytesPerPixel);

		//Main part of the image
		memcpy(PaddedImagedata + (i + padH) * (width + 2 * padW) * BytesPerPixel + padW * BytesPerPixel,
				Imagedata + (i * width) * BytesPerPixel,
				width * BytesPerPixel);

		//Right padded rows
		memcpy( PaddedImagedata + ((i + padH + 1) * (width + 2 * padW) * BytesPerPixel) - padW * BytesPerPixel,
				PaddedImagedata + ((i + padH + 1) * (width + 2 * padW) * BytesPerPixel) - 2 * padW * BytesPerPixel,
				padW * BytesPerPixel);
	}

	for(int i = 0; i < padH;i ++) {
		//Top padded rows
		memcpy(PaddedImagedata + i * (width + 2 * padW) * BytesPerPixel,
				PaddedImagedata + padH * (width + 2 * padW) * BytesPerPixel,
				(width + 2 * padW) * BytesPerPixel);

		//Bottom padded rows
		memcpy(PaddedImagedata + (height + padH + i) * (width + 2 * padW) * BytesPerPixel,
				PaddedImagedata + (height + padH - 1) * (width + 2 * padW) * BytesPerPixel,
				(width + 2 * padW) * BytesPerPixel);
	}
}

/*Main function*/
int main(int argc, char *argv[]) {
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, width, height, padW = WINDOW_SIZE / 2, padH = WINDOW_SIZE / 2;

	// Check for proper syntax
	if (argc < 2){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw" << endl;
		return 0;
	}

	cout << "Please enter Bytes Per Pixel : ";
	cin >> BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> width;
	cout << "Please enter Height of input image: ";
	cin >> height;

	// Allocate image data array
	unsigned char *Imagedata, *PaddedImagedata;

	// Allocation of image and padded image
	Imagedata       = new (nothrow) unsigned char[width * height * BytesPerPixel];
	PaddedImagedata = new (nothrow) unsigned char[(width + 2 * padW) * (height + 2 * padH) * BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	// Get the padded image for processing
	padding_image(Imagedata, PaddedImagedata, height, width, padH, padW, BytesPerPixel);

#ifdef MEAN
	// Apply mean filtering
	mean_filtering(Imagedata, PaddedImagedata, padW, padH, width, height, BytesPerPixel);
#endif

#ifdef MEDIAN
	// Apply median filtering
	median_filtering(Imagedata, PaddedImagedata, padW, padH, width, height, BytesPerPixel);
#endif

#ifdef GAUSSIAN
	// Apply gaussian filtering
	gaussian_filtering(Imagedata, PaddedImagedata, padW, padH, width, height, BytesPerPixel);
#endif

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen("MedianImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	return 0;
}
