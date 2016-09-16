// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int width, height, count[255] = {0};
	vector<float> probability, cdf;
	vector<int> final_bit;

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
	unsigned char *Imagedata;

	Imagedata = new (nothrow) unsigned char[width * height * BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			count[int(*(Imagedata + (j + i * width) * BytesPerPixel))]++;
		}
	}

	for(int i = 0; i < 255; i++) {
		probability.push_back(float(count[i]) / float(width * height));
		if(i > 0)
			cdf.push_back(cdf[i-1] + probability[i]);
		else
			cdf.push_back(probability[i]);

		final_bit.push_back(cdf.back() * 255);
	}

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			*(Imagedata + (j + i * width) * BytesPerPixel) = final_bit.at(int(*(Imagedata + (j + i * width) * BytesPerPixel)));
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	return 0;
}
