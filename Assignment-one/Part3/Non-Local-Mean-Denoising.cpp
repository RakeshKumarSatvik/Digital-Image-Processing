/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016
	
	File            : This code is for Non-local means noise removal algorithm.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <math.h>
#include <algorithm>

//Window size
#define WIN_SIZE 7

//Patch size
#define PATCH_SIZE 5
using namespace std;

/*
	Function Calculate_gaussian_values:
	input  : count  = Window size of the kernel for gaussian filter
	output : vector = the values generated with the  
	
	Description: This function takes the input of the kernel size needed to be 
	generated and gives back the vecotr of values.
*/
vector<float> Calculate_gaussian_values(int count) {
	vector<float> values;
	float temp, sigma = 0.88;
	int shift = PATCH_SIZE / 2;

	for(int i = 0; i < count; i++) {
		for(int j = 0; j < count; j++) {
			temp = (1 / (sqrt(2 * M_PI) * sigma)) * exp (- (float(((shift - i) * (shift - i)) + ((shift - j) * (shift - j))) / (2.0 * sigma * sigma)));
			values.push_back(temp);
		}
	}
	return values;
}

/*
	Function CalculatePatchVlaue:
	input  : Imagedata  	= input image
			 im				= local pixel height index
			 in				= local pixel width index
			 jm				= non-local pixel height index
			 jn				= non-local pixel width index
			 k				= channel value
			 width			= width of the image
			 height			= height of the image
			 BytesPerPixel	= Bytes per pixel in the image
			 padW			= padding done for the width
			 
	output : value			= returns the summation of the patch value of the patch for each pixel in the window
	
	Description: This function takes the input of the kernel size needed to be 
	generated and gives back the vecotr of values.
*/
float CalculatePatchVlaue(unsigned char *Imagedata, int in, int im, int jn, int jm, int k, int width, int height, int BytesPerPixel, int padW) {
	float value;
	vector<float> sum, gaussian_values;

	im += padW;
	in += padW;

	// Bringing the co-ordinates to the center
	im = im - PATCH_SIZE / 2;
	in = in - PATCH_SIZE / 2;

	// Loop through the patch size for the given pixel, calculate the neighborhood squared difference
	// between the local and non-local pixel. Sum them up and store it in a vector.
	for(int i = 0; i < PATCH_SIZE; i++) {
		for(int j = 0; j < PATCH_SIZE; j++) {
			value = int(*(Imagedata + (im + in * (width + 2 * padW)) * BytesPerPixel + k) + (j + i * PATCH_SIZE) * BytesPerPixel) -
						int(*(Imagedata + (jm + jn * (width + 2 * padW)) * BytesPerPixel + k) + (j + i * PATCH_SIZE) * BytesPerPixel);

			value *= value;
			sum.push_back(value);
		}
	}
	
	// Calculate the gaussian filter values for a given kernel size
	gaussian_values = Calculate_gaussian_values(PATCH_SIZE);

	value = 0;
	// Apply gaussian filter for the squared difference neighborhood
	for(unsigned int i = 0; i < sum.size(); i++) {
		value += sum.at(i) * gaussian_values.at(i);
	}
	return value;
}

/* Main function*/
int main(int argc, char *argv[]) {
	// Define file pointer and variables
	
	// To read/write from/to file
	FILE *file;
	int BytesPerPixel, width, height, padW = PATCH_SIZE / 2, padH = PATCH_SIZE / 2;
	int window_m, window_n;
	float constH = 150.0, constZ, value, temp;

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


	// loop through each channel in the image
	for(int k = 0; k < BytesPerPixel; k++) {
		// loop through the height of the image
		for(int i = 0; i < height; i++) {
			// loop through the width of the image
			for(int j = 0; j < width; j++) {
				//Window size and truncate if it is going out of bounds
				if(i < WIN_SIZE and j < WIN_SIZE) {
					window_m = 0;
					window_n = 0;
				} else if(i < WIN_SIZE) {
					window_m = 0;
					window_n = j - WIN_SIZE;
				} else if(j < WIN_SIZE) {
					window_m = i - WIN_SIZE;
					window_n = 0;
				} else {
					window_m = i - WIN_SIZE;
					window_n = j - WIN_SIZE;
				}

				value = 0;
				constZ = 0;
				// Depending on the window size, loop through the entire window 
				// and for each pixel of interest calculate the patch value depending on the patch size
				// detemine the difference of local and non-local pixels
				for(int m = window_m; m < window_m + WIN_SIZE; m++) {
					for(int n = window_n; n < window_n + WIN_SIZE; n++) {
						temp = CalculatePatchVlaue(PaddedImagedata, i, j, m, n, k, width, height, BytesPerPixel, padW);
						constZ += exp(-(temp / (constH * constH)));
						value  += exp(-(temp / (constH * constH))) * float(*(PaddedImagedata + ((n + padW) + (m + padH) * (width + 2 * padW)) * BytesPerPixel + k));
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = int((1 / constZ) * value);
			}
			cout << "Channel " << k << " Row " << i << " completed" << endl;
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen("NoiseRemovedNLM.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	return 0;
}
