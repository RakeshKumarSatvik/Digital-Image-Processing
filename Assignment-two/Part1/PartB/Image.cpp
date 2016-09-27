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
	this->prev_x            = 0;
	this->prev_y            = 0;
	this->Imagedata     	= NULL;
	this->SecondImagedata   = NULL;
	this->ThirdImagedata    = NULL;
	this->OutputImagedata   = NULL;
	this->RotatedImagedata  = NULL;
	this->ResizeImagedata   = NULL;
	this->CroppedImagedata  = NULL;
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
	this->RotatedImagedata = new (nothrow) unsigned char[this->hypotenuse * this->hypotenuse * this->BytesPerPixel];

	this->SecondImagedata  = new (nothrow) unsigned char[512 * 512 * this->BytesPerPixel];
	this->ThirdImagedata   = new (nothrow) unsigned char[512 * 512 * this->BytesPerPixel];
	this->ResizeImagedata  = new (nothrow) unsigned char[100 * 100 * this->BytesPerPixel];

	memset(this->RotatedImagedata, 255 , sizeof(unsigned char) * this->hypotenuse * this->hypotenuse * this->BytesPerPixel);
}

void Image::read_image(char *filename, char *filename2, char *filename3) {
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
	fread(this->SecondImagedata, sizeof(unsigned char), 512 * 512 * this->BytesPerPixel, file);
	fclose(file);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(filename3,"rb"))) {
		cout << "Cannot open file: " << filename <<endl;
		exit(1);
	}
	fread(this->ThirdImagedata, sizeof(unsigned char), 512 * 512 * this->BytesPerPixel, file);
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

vector<pair<int,int> > Image::get_edge_coordinates(int start_x, int start_y, unsigned char *data, int width, int height, int second) {
	vector<pair<int,int> > edges;
	pair<int,int> first_pixel, second_pixel, third_pixel, fourth_pixel;
	int flag = 1, count = 0, done = 1;
	int least_i, least_j, max_i, max_j;

	for(int k = 0;k < this->BytesPerPixel; k++) {
		for(int i = start_y; i < height; i++) {
			count = 0;
			for(int j = start_x; j < width; j++) {
				if(*(data + (j + (i * width)) * this->BytesPerPixel + k) != 255 && done) {
					if(flag) {
						least_i = i;
						least_j = j;
						max_i = i;
						max_j = j;
						flag = 0;
					}
					if(i < least_i) {
						least_i = i;
					}
					if(j < least_j) {
						least_j = j;
					}
					if(i > max_i) {
						max_i = i;
					}
					if(j > max_j) {
						max_j = j;
					}
				} else {
					count++;
				}
			}
			if(count == width && !flag) {
				done = 0;
			}
		}
	}
	first_pixel  = make_pair(least_j, least_i);
	second_pixel = make_pair(max_j, least_i);
	third_pixel  = make_pair(least_j, max_i);
	fourth_pixel = make_pair(max_j, max_i);

	edges.push_back(first_pixel);
	edges.push_back(second_pixel);
	edges.push_back(third_pixel);
	edges.push_back(fourth_pixel);

//	cout << edges.size() << " edges populated" << endl;
	return edges;
}

void Image::image_rotation(int start_x, int start_y, float angle) {
	double xprime, yprime, centre_x, centre_y;
	vector<pair<int,int> > edges;
	pair<int,int> first_pixel, second_pixel, third_pixel, fourth_pixel;

	// Will return 4 points, in the order of first, fourth, third, second
	//  first------------------second
	//	 [							]
	//	 [							]
	//	 [							]
	//	 [							]
	//  third-------------------fourth
	edges = Image::get_edge_coordinates(start_x, start_y, this->Imagedata, this->width, this->height, 0);

	fourth_pixel = edges.back();
	edges.pop_back();

	third_pixel = edges.back();
	edges.pop_back();

	second_pixel = edges.back();
	edges.pop_back();

	first_pixel = edges.back();
	edges.pop_back();

//	cout << "1 First " << first_pixel.first << " Second " << first_pixel.second << endl;
//	cout << "2 First " << second_pixel.first << " Second " << second_pixel.second << endl;
//	cout << "3 First " << third_pixel.first << " Second " << third_pixel.second << endl;
//	cout << "4 First " << fourth_pixel.first << " Second " << fourth_pixel.second << endl;

	centre_x = (first_pixel.first + fourth_pixel.first) / 2;
	centre_y = (first_pixel.second + fourth_pixel.second) / 2;
	for(int k = 0; k < this->BytesPerPixel; k++) {
		for(float i = first_pixel.second; i <= fourth_pixel.second; i++) {
			for(float j = first_pixel.first; j <= fourth_pixel.first; j++) {
				j = j - centre_x;
				i = i - centre_y;

				xprime = j * cos(angle * M_PI / 180.0) - i * sin(angle * M_PI / 180.0);
				yprime = j * sin(angle * M_PI / 180.0) + i * cos(angle * M_PI / 180.0);

				xprime = xprime + centre_x;
				yprime = yprime + centre_y;

				j = j + centre_x;
				i = i + centre_y;

//				cout << "x " << xprime << " " << yprime << " j " << j << " i " << i << endl;
				if(xprime - int(xprime) != 0 || yprime - int(yprime) != 0) {
					*(this->RotatedImagedata + (int(j) + (int(i) * this->hypotenuse)) * this->BytesPerPixel + k) =
							Image::bilinear_interpolation(this->Imagedata, xprime, yprime, k, this->width);
				} else {
					*(this->RotatedImagedata + (int(j) + (int(i) * this->hypotenuse)) * this->BytesPerPixel + k) =
						*(this->Imagedata + (int(xprime) + (int(yprime) * this->width)) * this->BytesPerPixel + k);
				}
			}
		}
	}

//	this->write_image("Debug\\rotated.raw",this->RotatedImagedata, this->hypotenuse, this->hypotenuse);
}

void Image::resize_image(int EnhX, int EnhY, int SizeX, int SizeY) {
	float pPrime, qPrime;

	for(int j = 0; j < EnhY; j++) {
		for(int k = 0; k < EnhX; k++) {
			pPrime = (float(SizeX - 1) / float(EnhX - 1)) * (k);
			qPrime = (float(SizeY - 1) / float(EnhY - 1)) * (j);

			if(pPrime - int(pPrime) != 0 || qPrime - int(qPrime) != 0) {
				/*Bilinear Implementation*/
				//Loop for the 24bit RBG values
				for(int i = 0; i < 3; i++) {
					*(this->ResizeImagedata + (k + j * EnhX) * BytesPerPixel + i) = this->bilinear_interpolation(this->CroppedImagedata, pPrime, qPrime, i, SizeX);
				}
			} else {
				//else if the points are integer values, then directly copy from the cropped image
				memcpy(this->ResizeImagedata + (k + j * EnhX) * this->BytesPerPixel,
						this->CroppedImagedata + (int(pPrime) + int(qPrime) * SizeX) * this->BytesPerPixel,
						this->BytesPerPixel);
			}
		}
	}
}

void Image::crop_image() {
	vector<pair<int,int> > edges;
	pair<int,int> first_pixel, second_pixel, third_pixel, fourth_pixel;
	int fill_width, fill_height;

	// Will return 4 points, in the order of first, fourth, third, second
	//  first------------------second
	//	 [							]
	//	 [							]
	//	 [							]
	//	 [							]
	//  third-------------------fourth
	edges = Image::get_edge_coordinates(this->prev_x, this->prev_y, this->RotatedImagedata, this->hypotenuse, this->hypotenuse, 1);

 	fourth_pixel = edges.back();
	edges.pop_back();

 	third_pixel = edges.back();
	edges.pop_back();

 	second_pixel = edges.back();
	edges.pop_back();

 	first_pixel = edges.back();
	edges.pop_back();

	this->prev_x = fourth_pixel.first + 1;
	this->prev_y = fourth_pixel.second + 1;

//	cout << "First " << first_pixel.first << " Second " << first_pixel.second << endl;
//	cout << "First " << second_pixel.first << " Second " << second_pixel.second << endl;
//	cout << "First " << third_pixel.first << " Second " << third_pixel.second << endl;
//	cout << "First " << fourth_pixel.first << " Second " << fourth_pixel.second << endl;

	fill_width  = fourth_pixel.first - first_pixel.first - 5;
	fill_height = fourth_pixel.second - first_pixel.second - 4;

	this->CroppedImagedata  = new (nothrow) unsigned char[fill_width * fill_height * this->BytesPerPixel];

	for(int i = 0; i < fill_height; i++) {
		memcpy(this->CroppedImagedata
					+ i * fill_width * BytesPerPixel, //Move the cursor in the output buffer
				this->RotatedImagedata
					+ (first_pixel.first + 3 + (first_pixel.second + 2) * this->hypotenuse) * BytesPerPixel //Take to the first line
					+ i * this->hypotenuse * BytesPerPixel, //Move to the specific line as per the loop
				(fill_width) * BytesPerPixel); //Size copied per each line
	}

//	cout << "fill width " << fill_width << " fill height " << fill_height << endl;
//	this->write_image("Debug\\cropped.raw",this->CroppedImagedata, fill_width, fill_height);
	this->resize_image(100, 100, fill_width, fill_height);
//	this->write_image("Debug\\resized.raw",this->ResizeImagedata, 100, 100);
}

void Image::fill_image(unsigned char *data) {
	for(int k = 0; k < this->BytesPerPixel; k++) {
		for(int i = 0; i < 512; i++) {
			for(int j = 0; j < 512; j++) {
				if(*(data+ (j + (i * 512)) * this->BytesPerPixel + k) == 255) {
					for(int ii = 0; ii < 100; ii++) {
						memcpy(data+ (j + ((i + ii) * 512)) * this->BytesPerPixel + k,
								this->ResizeImagedata + ii * 100 * this->BytesPerPixel + k,
								100 * this->BytesPerPixel);
					}
					return;
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
