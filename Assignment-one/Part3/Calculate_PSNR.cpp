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

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, width, height, value;
	double mse, psnr, sum = 0;

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
	unsigned char *Imagedata, *SecondImagedata;

	Imagedata       = new (nothrow) unsigned char[width * height * BytesPerPixel];
	SecondImagedata = new (nothrow) unsigned char[width * height * BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen("C:\\Users\\RakeshKumarSatvik\\workspace\\EE569-1-P3-A\\Debug\\Lena.raw ","rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen("C:\\Users\\RakeshKumarSatvik\\workspace\\EE569-1-P3-A\\Debug\\Lena_noisy.raw ","rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(SecondImagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	for(int k = 0; k < BytesPerPixel; k++) {
		sum = 0;
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				value = int(*(SecondImagedata + (j + i * width) * BytesPerPixel + k)) - int(*(Imagedata + (j + i * width) * BytesPerPixel + k));
				value *= value;
				sum += value;
			}
		}
		mse = float(sum) / float(height * width);

		psnr = 10 * log10(float(255 * 255) / mse);

		cout << psnr << endl;
	}

	return 0;
}
