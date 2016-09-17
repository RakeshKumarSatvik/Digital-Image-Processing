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

using namespace std;

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, count[256] = {0};
	int width, height, count_of_pixels, no_of_pixels[256] = {0}, tmpH, tmpW, tmpI;
	vector<vector<pair<int,int> > > store_values;
	pair<int, int> temp;
	vector<pair<int,int> > initial;

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

	cout << "Bytes Per Pixel : " << BytesPerPixel << " Width :" << width << " Height :" << height << endl;
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

	count_of_pixels = width * height / 256;
	cout << "count of pixels : " << count_of_pixels << endl;

	for(int k = 0; k < BytesPerPixel; k++) {
		store_values.clear();
		initial.push_back(make_pair(0,0));
		memset(count, 0, sizeof(int) * 256);
		memset(no_of_pixels, 0, sizeof(int) * 256);

		for(int i = 0; i < 256; i++) {
			store_values.push_back(initial);
		}

		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				count[int(*(Imagedata + (j + i * width) * BytesPerPixel + k))]++;
				store_values.at(int(*(Imagedata + (j + i * width) * BytesPerPixel + k))).push_back(make_pair(j, i));
			}
		}

		tmpI = 0;
		for(int i = 0; i < 256; i++) {
			for(int j = 1; j <= count[i]; j++) {
				tmpW = store_values.at(i).at(j).first;
				tmpH = store_values.at(i).at(j).second;
				no_of_pixels[tmpI]++;
				if(no_of_pixels[tmpI] <= count_of_pixels) {
					*(Imagedata + (tmpW + tmpH * width) * BytesPerPixel + k) = tmpI;
				} else {
					*(Imagedata + (tmpW + tmpH * width) * BytesPerPixel + k) = tmpI++;
				}
			}
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
