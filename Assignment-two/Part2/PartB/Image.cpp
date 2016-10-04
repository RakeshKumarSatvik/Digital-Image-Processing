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

#define M_SIZE 3

int floyd_stein[3][3] = {
	{0, 0, 0},
	{0, 0, 7},
	{3, 5, 1}
};

int jjn[5][5] = {
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 7, 5},
		{3, 5, 7, 5, 3},
		{1, 3, 5, 3, 1}
};

int stucki[5][5] = {
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 8, 4},
		{2, 4, 8, 4, 2},
		{1, 2, 4, 2, 1}
};

Image::Image() {
	// TODO Auto-generated constructor stub
	this->height 			= 0;
	this->width  			= 0;
	this->BytesPerPixel 	= 1;
	this->Imagedata     	= NULL;
	this->PaddedImagedata   = NULL;
}

Image::~Image() {
	// TODO Auto-generated destructor stub
	delete(this->Imagedata);
}

void Image::allocate_image() {
	int padw, padh;

	padw = M_SIZE/2;
	padh = M_SIZE/2;

	cout << "Please enter Bytes Per Pixel : ";
	cin >> this->BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> this->width;
	cout << "Please enter Height of input image: ";
	cin >> this->height;

	// Allocation of image and padded image
	this->Imagedata 	   = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->PaddedImagedata  = new (nothrow) unsigned char[(this->width + 2 * padw) * (this->height + 2 * padh) * this->BytesPerPixel];
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

void Image::padding_image(int padH, int padW) {
	for(int i = 0; i < this->height; i++) {
		//Left padded rows
		memcpy(this->PaddedImagedata + (i + padH) * (this->width + 2 * padW) * this->BytesPerPixel,
				this->Imagedata + (i * this->width) * this->BytesPerPixel,
				padH * this->BytesPerPixel);

		//Main part of the image
		memcpy(this->PaddedImagedata + (i + padH) * (this->width + 2 * padW) * this->BytesPerPixel + padW * this->BytesPerPixel,
				this->Imagedata + (i * this->width) * this->BytesPerPixel,
				this->width * this->BytesPerPixel);

		//Right padded rows
		memcpy( this->PaddedImagedata + ((i + padH + 1) * (this->width + 2 * padW) * this->BytesPerPixel) - padW * this->BytesPerPixel,
				this->PaddedImagedata + ((i + padH + 1) * (this->width + 2 * padW) * this->BytesPerPixel) - 2 * padW * this->BytesPerPixel,
				padW * this->BytesPerPixel);
	}

	for(int i = 0; i < padH;i ++) {
		//Top padded rows
		memcpy(this->PaddedImagedata + i * (this->width + 2 * padW) * this->BytesPerPixel,
				this->PaddedImagedata + padH * (this->width + 2 * padW) * this->BytesPerPixel,
				(this->width + 2 * padW) * this->BytesPerPixel);

		//Bottom padded rows
		memcpy(this->PaddedImagedata + (this->height + padH + i) * (this->width + 2 * padW) * this->BytesPerPixel,
				this->PaddedImagedata + (this->height + padH - 1) * (this->width + 2 * padW) * this->BytesPerPixel,
				(width + 2 * padW) * BytesPerPixel);
	}
}


void Image::error_diffusion() {
	int padw, padh, temp, compare, start, inc;
	unsigned char *pixel, *dest;

	padw = M_SIZE/2;
	padh = M_SIZE/2;

	this->padding_image(padw, padh);

	cout << "padw " << padw << " padh " << padh << endl;

	for(int i = 0; i < this->height; i++) {
		if(i % 2 == 1 && M_SIZE == 3) {
			start = this->width;
			inc = -1;
		} else {
			start = 0;
			inc = 1;
		}

		for(int j = start; j < this->width; j+= inc) {
			pixel = this->PaddedImagedata + (j + padw + (i + padh) * (this->width + 2 * padw)) * this->BytesPerPixel;
			compare = int(*pixel);

			for(int m = 0; m < M_SIZE; m++) {
				for(int n = 0; n < M_SIZE; n++) {
					dest = this->PaddedImagedata + (j + n + (i + m) * (this->width + 2 * padw)) * this->BytesPerPixel;

					if(compare > 127) {
						*pixel = 255;
						temp = (compare - 255);
						*dest += ((temp * floyd_stein[m][n]) / 48);
					} else {
						*pixel = 0;
						temp = (compare - 0);
						*dest += ((temp * floyd_stein[m][n]) / 48);
					}
				}
			}
		}
	}
}

void Image::write_image(char *filename, unsigned char *data, int inp_width, int inp_height) {
	// To read/write from/to file
	FILE *file;
	int padw, padh;

	padw = M_SIZE/2;
	padh = M_SIZE/2;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(data, sizeof(unsigned char), (inp_width + 2 * padw) * (inp_height + 2 * padh) * this->BytesPerPixel, file);
	fclose(file);
}
