/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016
	
	File            : This code is for Histogram equalization using bucket filling approach.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>

using namespace std;

/* Main Function */
int main(int argc, char *argv[]) {
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, count[256] = {0};
	int width, height, count_of_pixels, no_of_pixels[256] = {0}, tmpH, tmpW, tmpI;
	vector<vector<pair<int,int> > > store_values;
	pair<int, int> temp;
	vector<pair<int,int> > initial;

	// Check for proper syntax
	if (argc < 2){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw" << endl;
		return 0;
	}

	cout << "Please enter Bytes Per Pixel : ";
	cin >> BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> width;
	cout << "Please enter Height of input image: ";
	cin >> height;

	cout << "Bytes Per Pixel : " << BytesPerPixel << " Width :" << width << " Height :" << height << endl;
	// Allocate image data array
	unsigned char *Imagedata;

	// Allocation of image
	Imagedata = new (nothrow) unsigned char[width * height * BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	count_of_pixels = width * height / 256;
	//cout << "count of pixels : " << count_of_pixels << endl;

	// Loop through each channel in the input image	
	for(int k = 0; k < BytesPerPixel; k++) {
		store_values.clear();
		initial.push_back(make_pair(0,0));
		memset(count, 0, sizeof(int) * 256);
		memset(no_of_pixels, 0, sizeof(int) * 256);

		// Initialize to 0,0
		for(int i = 0; i < 256; i++) {
			store_values.push_back(initial);
		}

		// Loop through the height of the input image		
		for(int i = 0; i < height; i++) {
			// Loop through the width of the input image
			for(int j = 0; j < width; j++) {
				// Count the number of pixels with each intensity values
				count[int(*(Imagedata + (j + i * width) * BytesPerPixel + k))]++;
				store_values.at(int(*(Imagedata + (j + i * width) * BytesPerPixel + k))).push_back(make_pair(j, i));
			}
		}

		tmpI = 0;
		// Loop through each pixel intensity value
		for(int i = 0; i < 256; i++) {
			// Loop through each pixel with that intensity value
			for(int j = 1; j <= count[i]; j++) {
				// Pop the height and width index of that particular pixel
				tmpW = store_values.at(i).at(j).first;
				tmpH = store_values.at(i).at(j).second;
				no_of_pixels[tmpI]++;
				// If the number of pixels in the bucket exceed the limit, go to the next bucket
				if(no_of_pixels[tmpI] <= count_of_pixels) {
					*(Imagedata + (tmpW + tmpH * width) * BytesPerPixel + k) = tmpI;
				} else {
					*(Imagedata + (tmpW + tmpH * width) * BytesPerPixel + k) = tmpI++;
				}
			}
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen("HistogramMethodB.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	return 0;
}
