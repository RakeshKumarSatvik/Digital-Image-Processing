/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016
	
	File            : This code is for Histogram equalization using gaussian filter match.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <math.h>
#include <algorithm>

using namespace std;

/* Main function*/
int main(int argc, char *argv[]) {
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel, normalized_value, min, max;
	int width, height, count[256] = {0};
	float temp;
	vector<float>::iterator result;
	vector<float> probability, cdf;
	vector<int> initial;
	vector<vector<int> > final_bit;

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

	// Loop through each channel in the input image
	for(int k = 0; k < BytesPerPixel; k++) {
		final_bit.push_back(initial);
		cdf.clear();
		probability.clear();
		memset(count, 0, sizeof(int) * 256);

		// Loop through each pixel intensity value
		for(int i = 0; i < 256; i++) {
			// Apply the formula
			float num1   = float((i - 125.0) * (i - 125.0));
			float num    = num1 / (2.0 * 40.0 * 40.0);
			float numexp = exp(-num);
			float den    = float(sqrt(2 * M_PI) * 40.0);

			temp = (float(1) / den) * numexp;
			
			// Get the probability
			probability.push_back(temp);

			// Get the cumulative probability
			if(i > 0)
				cdf.push_back(cdf[i-1] + probability[i]);
			else
				cdf.push_back(probability[i]);

		}
		
		// Depending on the range, determine the max and min for normalization
		result = max_element(cdf.begin(), cdf.end());
		max    = distance(cdf.begin(), result);

		result = min_element(cdf.begin(), cdf.end());
		min    = distance(cdf.begin(), result);

		//cout << max << " " << min << endl;
		// Loop through each pixel intensity value		
		for(int i = 0; i < 256; i++) {
			normalized_value = cdf.at(i) * 255;

			// Normalization of the output
			normalized_value = int((float(normalized_value - min) / float(max - min)) * 255.0);

			// Store the output in a vector of range 0 - 255
			final_bit.at(k).push_back(normalized_value);
		}

		// Loop through the height of the input image
		for(int i = 0; i < height; i++) {
			// Loop through the width of the input image
			for(int j = 0; j < width; j++) {
				*(Imagedata + (j + i * width) * BytesPerPixel + k) = final_bit.at(k).at(int(*(Imagedata + (j + i * width) * BytesPerPixel + k)));
			}
		}
	}
	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen("HistogramGaussian.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	return 0;
}
