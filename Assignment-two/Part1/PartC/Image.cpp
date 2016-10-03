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

using namespace std;

Image::Image() {
	// TODO Auto-generated constructor stub
	this->height 			= 0;
	this->width  			= 0;
	this->BytesPerPixel 	= 1;
	this->Imagedata     	= NULL;
	this->SecondImagedata   = NULL;
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
	this->SecondImagedata  = new (nothrow) unsigned char[350 * 146 * this->BytesPerPixel];
}

void Image::read_image(char *filename, char *filename2) {
	// To read/write from/to file
	FILE *file;

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(filename,"rb"))) {
		cout << "Cannot open file: " << filename <<endl;
		exit(1);
	}
	fread(this->Imagedata, sizeof(unsigned char), this->width * this->height * this->BytesPerPixel, file);
	fclose(file);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(filename2,"rb"))) {
		cout << "Cannot open file: " << filename <<endl;
		exit(1);
	}
	fread(this->SecondImagedata, sizeof(unsigned char), 350 * 146 * this->BytesPerPixel, file);
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

void Image::image_overlay() {
	double x2, y2, w2;
	vector<double> matrix_h;
	int temp;

	matrix_h.push_back(1.1141);
	matrix_h.push_back(0.8871);
	matrix_h.push_back(342.0565);
	matrix_h.push_back(-0.1676);
	matrix_h.push_back(0);
	matrix_h.push_back(612.4638);
	matrix_h.push_back(0.0009);
	matrix_h.push_back(0);
	matrix_h.push_back(1);

	for(int k = 0; k < BytesPerPixel; k++) {
		for(int i = 0; i < 146; i++) {
			for(int j = 0; j < 350; j++) {
				x2 = matrix_h.at(0) * j + matrix_h.at(1) * i + matrix_h.at(2);
				y2 = matrix_h.at(3) * j + matrix_h.at(4) * i + matrix_h.at(5);
				w2 = matrix_h.at(6) * j + matrix_h.at(7) * i + matrix_h.at(8);

				x2 /= w2;
				y2 /= w2;

//				cout << " x2 " << x2 << " y2 " << y2 << " i " << i << " j " << j << endl;

				if(x2 - int(x2) != 0 || y2 - int(y2) != 0) {
					temp = this->bilinear_interpolation(this->SecondImagedata, j, i, k, 350);
					if(temp && temp != 255) {
						*(this->Imagedata + (int(x2) + int(y2) * this->width) * BytesPerPixel + k) = temp;
					}
				} else {
					if(*(this->SecondImagedata + (j + i * 350) * BytesPerPixel + k) != 0 &&
							*(this->SecondImagedata + (j + i * 350) * BytesPerPixel + k) != 255) {
						*(this->Imagedata + (int(x2) + int(y2) * this->width) * BytesPerPixel + k) =
								*(this->SecondImagedata + (j + i * 350) * BytesPerPixel + k);
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
