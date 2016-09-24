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
#include <string.h>

#include "Image.h"

using namespace std;

Image::Image() {
	// TODO Auto-generated constructor stub
	this->height 			= 0;
	this->width  			= 0;
	this->BytesPerPixel 	= 1;
	this->Imagedata     	= NULL;
	this->OutputImagedata   = NULL;
}

Image::~Image() {
	// TODO Auto-generated destructor stub
	delete(this->Imagedata);
	delete(this->OutputImagedata);
}

void Image::allocate_image() {
	cout << "Please enter Bytes Per Pixel : ";
	cin >> this->BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> this->width;
	cout << "Please enter Height of input image: ";
	cin >> this->height;

	// Allocation of image and padded image
	this->Imagedata 	  = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->OutputImagedata = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];

	memset(this->OutputImagedata, 0 , sizeof(unsigned char) * this->width * this->height * this->BytesPerPixel);
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

void Image::warped_image() {
	float jump, temp;

	for(int k = 0; k < this->BytesPerPixel; k++) {
		for(int i = 0; i < this->height / 2; i++) {
			for(int j = 0; j <= i; j++) {
				if(i != 0)
					jump = float(this->width / 2) / float(i);
				else
					jump = float(this->width / 2) / float(i + 1);

				if(jump - int(jump) != 0) {
					// First triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width - j) * this->BytesPerPixel + k) =
							this->bilinear_interpolation(float(this->width / 2 ) - float(j) * jump, i, k);

					// Second triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width + j) * this->BytesPerPixel + k) =
							this->bilinear_interpolation(float(this->width / 2 ) + float(j) * jump, i, k);

				} else {
					// First triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width - j) * this->BytesPerPixel + k) =
							*(this->Imagedata + ((this->width / 2 ) + i * this->width - j * int(jump)) * this->BytesPerPixel + k);

					// Second triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width + j) * this->BytesPerPixel + k) =
							*(this->Imagedata + ((this->width / 2 ) + i * this->width + j * int(jump)) * this->BytesPerPixel + k);
				}
			}
		}

		for(int i = this->height; i >= (this->height / 2); i--) {
			for(int j = 0; j <= (this->height - i); j++) {
				temp = this->height - i;
				if(temp != 0)
					jump = float(this->width / 2) / float(temp);
				else
					jump = float(this->width / 2) / float(temp + 1);

				if(jump - int(jump) != 0) {
					// Third triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width - j) * this->BytesPerPixel + k) =
							this->bilinear_interpolation(float(this->width / 2 ) - float(j) * jump, i, k);

					// Fourth triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width + j) * this->BytesPerPixel + k) =
							this->bilinear_interpolation(float(this->width / 2 ) + float(j) * jump, i, k);

				} else {
					// Third triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width - j) * this->BytesPerPixel + k) =
							*(this->Imagedata + ((this->width / 2 ) + i * this->width - j * int(jump)) * this->BytesPerPixel + k);

					// Fourth triangle
					*(this->OutputImagedata + ((this->width / 2 ) + i * this->width + j) * this->BytesPerPixel + k) =
							*(this->Imagedata + ((this->width / 2 ) + i * this->width + j * int(jump)) * this->BytesPerPixel + k);
				}
			}
		}

	}
}

int Image::bilinear_interpolation(float pPrime, int qPrime, int k) {
	/*Bilinear Implementation*/
	int BIx, BIy;
	int numerator1, numerator2, numerator;
	BIx = int(pPrime);
	BIy = int(qPrime);

	numerator1 = ((BIx + 1 - pPrime) * *(this->Imagedata + ((BIx + BIy * this->width) * BytesPerPixel) + k) +
						(pPrime - BIx) * *(this->Imagedata + (((BIx + 1) + BIy * this->width) * BytesPerPixel) + k)) *
								(BIy + 1 - qPrime);

	numerator2 = ((BIx + 1 - pPrime) * *(this->Imagedata + ((BIx + (BIy + 1) * this->width) * BytesPerPixel) + k) +
					(pPrime - BIx) * *(this->Imagedata + (((BIx + 1 )+ (BIy + 1) * this->width) * BytesPerPixel) + k)) *
								(qPrime - BIy);

	numerator   = numerator1 + numerator2;

	return numerator;
}

void Image::write_image(char *filename) {
	// To read/write from/to file
	FILE *file;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(this->OutputImagedata, sizeof(unsigned char), this->width * this->height * this->BytesPerPixel, file);
	fclose(file);
}
