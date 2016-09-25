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
	this->hypotenuse		= 0;
	this->Imagedata     	= NULL;
	this->OutputImagedata   = NULL;
	this->RotatedImagedata  = NULL;
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

	this->hypotenuse = ceil(sqrt(pow(this->width,2) + pow(this->height,2)));
	// Allocation of image and padded image
	this->Imagedata 	   = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->OutputImagedata  = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->RotatedImagedata = new (nothrow) unsigned char[this->hypotenuse  *  this->hypotenuse  * this->BytesPerPixel];

	cout << "Height " << this->hypotenuse << " Width " << this->hypotenuse << endl;
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

vector<pair<int,int> > Image::get_edge_coordinates() {
	vector<pair<int,int> > edges;
	pair<int,int> prev_pixel;
	int flag = 1, count = 0, done = 1;

	for(int k = 0;k < this->BytesPerPixel; k++) {
		for(int i = 0; i < this->height; i++) {
			for(int j = 0; j < this->width; j++) {
				if(*(this->Imagedata + (j + (i * this->width)) * this->BytesPerPixel + k) != 255) {
					if(flag && done) {
						edges.push_back(make_pair(j,i));
						flag = 0;
					}
					prev_pixel = make_pair(j,i);
				}
				if(*(this->Imagedata + (j + (i * this->width)) * this->BytesPerPixel + k) == 255 && flag == 0) {
					count++;
				}
			}
			if(count == this->width && done) {
				edges.push_back(prev_pixel);
				done = 0;
			}
			count = 0;
		}
	}

	cout << edges.size() << " edges populated" << endl;
	return edges;
}

void Image::image_rotation(int angle) {
//	float xprime, yprime;
	vector<pair<int,int> > edges;
	pair<int,int> prev_pixel;

	edges = Image::get_edge_coordinates();

 	prev_pixel = edges.back();
	edges.pop_back();
	cout << "First " << prev_pixel.first << " Second " << prev_pixel.second << endl;
	prev_pixel = edges.back();
	edges.pop_back();
	cout << "First " << prev_pixel.first << " Second " << prev_pixel.second << endl;

//	for(int k = 0; k < BytesPerPixel; k++) {
//		for(int i = 0; i < this->height; i++) {
//			for(int j = 0; j < this->width; j++) {
//				xprime = j * cos(angle * M_PI / 180) - i * sin(angle * M_PI / 180);
//				yprime = j * sin(angle * M_PI / 180) + i * cos(angle * M_PI / 180);
////				cout << " x " << xprime << " y " << yprime << " i " << i << " j " << j << endl;
//				if(*(this->Imagedata + (j + (i * this->width)) * this->BytesPerPixel + k) != 255) {
//					if(xprime < this->width and yprime < this->width and xprime > 0 and yprime > 0) {
//						*(this->OutputImagedata + (int(xprime) + (int(yprime) * this->width)) * this->BytesPerPixel + k) =
//							*(this->Imagedata + (j + (i * this->width)) * this->BytesPerPixel + k);
//					}
//				}
//			}
//		}
//	}
}

void Image::write_image(char *filename) {
	// To read/write from/to file
	FILE *file;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(this->OutputImagedata, sizeof(unsigned char), this->hypotenuse * this->hypotenuse * this->BytesPerPixel, file);
	fclose(file);
}
