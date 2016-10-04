/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016

	File            : This code is for Non-local means noise removal algorithm.
*/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include "Image.h"
#include <string.h>
#include <stdlib.h>

using namespace std;

int dither_2[2][2] = {
	{0, 2},
	{3, 1}
};

int dither_4[4][4] = {
		{0,  8,  2, 10},
		{12, 4, 14,  6},
		{3, 11,  1,  9},
		{15, 7, 13,  5}
};

int dither_8[8][8] = {
	{ 0, 32, 8,  40, 2,  34, 10, 42},
	{48, 16, 56, 24, 50, 18, 58, 26},
	{12, 44, 4,  36, 14, 46, 6,  38},
	{60, 28, 52, 20, 62, 30, 54, 22},
	{ 3, 35, 11, 43, 1,  33, 9,  41},
	{51, 19, 59, 27, 49, 17, 57, 25},
	{15, 47, 7,  39, 13, 45, 5,  37},
	{63, 31, 55, 23, 61, 29, 53, 21}
};

Image::Image() {
	// TODO Auto-generated constructor stub
	this->height 			= 0;
	this->width  			= 0;
	this->BytesPerPixel 	= 1;
	this->Imagedata     	= NULL;
}

Image::~Image() {
	// TODO Auto-generated destructor stub
	delete(this->Imagedata);
}

void Image::allocate_image() {
	cout << "Please enter Bytes Per Pixel : ";
	cin >> this->BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> this->width;
	cout << "Please enter Height of input image: ";
	cin >> this->height;

	// Allocation of image and padded image
	this->Imagedata 	   = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
}

void Image::read_image(char *filename) {
	// To read/write from/to file
	FILE *file;

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(filename,"rb"))) {
		cout << "Cannot open file: " << filename <<endl;
		exit(1);
	}
	fread(this->Imagedata, sizeof(unsigned char), this->width * this->height * this->BytesPerPixel, file);
	fclose(file);
}

int Image::bilinear_interpolation(unsigned char *data, float pPrime, int qPrime, int k, int inp_width) {
	/*Bilinear Implementation*/
	int p, q;
	double a, b;
	int pixel;
	p = int(floor(pPrime));
	q = int(floor(qPrime));

	b = qPrime - q;
	a = pPrime - p;

	pixel = (1 - b) * ((1 - a) * *(data + (p + q * inp_width) * this->BytesPerPixel + k)
			+ a * *(data + ((p + 1) + q * inp_width) * this->BytesPerPixel + k)) +
			b * ((1 - a) * *(data + (p + (q + 1) * inp_width) * this->BytesPerPixel + k)
			+ a * *(data + ((p + 1) + (q + 1) * inp_width) * this->BytesPerPixel + k));
	return pixel;
}

void Image::dithering() {
	unsigned char *pixel;
	int size = 8;
	double threshold;


	for(int i = 0; i < this->height - size; i += size) {
		for(int j = 0; j < this->width - size; j += size) {

			for(int m = 0; m < size; m++) {
				for(int n = 0; n < size; n++) {
					pixel = this->Imagedata + (j + m + (i + n) * this->width) * this->BytesPerPixel;

					threshold = ((dither_8[m][n] + 0.5) / (size * size)) * 255;
					cout << threshold << " " << int(*pixel) << endl;
					if(*pixel > threshold) {
						*pixel = 0;
					} else {
						*pixel = 255;
					}
				}
			}
		}
	}
}

void Image::write_image(char *filename, unsigned char *data, int inp_width, int inp_height) {
	// To read/write from/to file
	FILE *file;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(data, sizeof(unsigned char), inp_width * inp_height * this->BytesPerPixel, file);
	fclose(file);
}
