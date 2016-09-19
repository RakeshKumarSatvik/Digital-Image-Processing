// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <math.h>
#include <algorithm>

using namespace std;

#define GAUSSIAN

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

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, width, height, padW = 1, padH = 1;
#ifndef MEDIAN
	int sum = 0;
#else
	vector<int> image_pixels;
#endif

	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
		return 0;
	}

	cout << "Bytes Per Pixel : ";
	cin >> BytesPerPixel;
	cout << "Width : ";
	cin >> width;
	cout << "Height : ";
	cin >> height;

	// Allocate image data array
	unsigned char *Imagedata, *PaddedImagedata;

	Imagedata       = new (nothrow) unsigned char[width * height * BytesPerPixel];
	PaddedImagedata = new (nothrow) unsigned char[(width + 2 * padW) * (height + 2 * padH) * BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen("C:\\Users\\RakeshKumarSatvik\\workspace\\EE569-1-P3-A\\Debug\\Lena_noisy.raw ","rb"))) {
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
		memcpy(PaddedImagedata + (i + padH) * (width + 2 * padW) * BytesPerPixel + padH * BytesPerPixel,
				Imagedata + (i * width) * BytesPerPixel,
				width * BytesPerPixel);

		//Right padded rows
		memcpy(PaddedImagedata + ((i + padH + 1) * (width + 2 * padW) - 1) * BytesPerPixel,
				Imagedata + ((i + 1) * width - 1) * BytesPerPixel,
				padH * BytesPerPixel);
	}

	for(int i = 0; i < padH;i ++) {
		//Top padded rows
		memcpy(PaddedImagedata + i * (width + 2 * padW) * BytesPerPixel,
				PaddedImagedata + padH * (width + 2 * padW) * BytesPerPixel,
				(width + 2 * padH) * BytesPerPixel);

		//Bottom padded rows
		memcpy(PaddedImagedata + (height + padH + i) * (width + 2 * padW) * BytesPerPixel,
				PaddedImagedata + height * (width + 2 * padW) * BytesPerPixel,
				(width + 2 * padH) * BytesPerPixel);
	}

#ifdef MEAN
	for(int k = 0; k < BytesPerPixel; k++) {
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				sum = 0;
				//Calculating the mean value
				for(int m = 0; m < 3; m++) {
					for(int n = 0; n < 3; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						sum += value;
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = sum / 9;
			}
		}
	}
#endif

#ifdef MEDIAN
	for(int k = 0; k < BytesPerPixel; k++) {
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				image_pixels.clear();
				//Calculating the mean value
				for(int m = 0; m < 3; m++) {
					for(int n = 0; n < 3; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						image_pixels.push_back(value);
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = int(CalcMedian(image_pixels));
			}
		}
	}
#endif

#ifdef GAUSSIAN
	for(int k = 0; k < BytesPerPixel; k++) {
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				sum = 0;
				//Calculating the mean value
				for(int m = 0; m < 3; m++) {
					for(int n = 0; n < 3; n++) {
						int value = *(PaddedImagedata + ((j + n) + (i + m) * (width + 2 * padH)) * BytesPerPixel + k);
						if((m == 0 && n == 1) or (m == 1 && n == 0) or (m == 2 && n == 1) or(m == 1 && n == 2))
							value *= 2;
						else if(m == 1 && n ==1)
							value *= 4;
						sum += value;
					}
				}
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = sum / 16;
			}
		}
	}
#endif

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[3],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(PaddedImagedata, sizeof(unsigned char), (width + padW) * (height + padH) * BytesPerPixel, file);
	fclose(file);
	return 0;
}
