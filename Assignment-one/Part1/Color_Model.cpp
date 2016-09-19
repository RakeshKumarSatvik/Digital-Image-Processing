/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016
	
	File            : This code is for getting HSL and CMY outputs.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int width, height, hmin, smin, lmin, hmax, smax, lmax;
	vector<float> temp1, temp2, temp3;
	float red, green, blue;
	float hslM, hslm2, hslC;

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
	unsigned char *Imagedata, *CyanImagedata, *MagentaImagedata, *YellowImagedata;
	unsigned char *HueImagedata, *SaturationImagedata, *LightImagedata;
	float         *tempHue, *tempSaturation, *tempLight;

	// Allocation of image
	Imagedata        	= new (nothrow) unsigned char[width * height * BytesPerPixel];
	CyanImagedata    	= new (nothrow) unsigned char[width * height];
	MagentaImagedata 	= new (nothrow) unsigned char[width * height];
	YellowImagedata  	= new (nothrow) unsigned char[width * height];

	HueImagedata    	= new (nothrow) unsigned char[width * height];
	SaturationImagedata = new (nothrow) unsigned char[width * height];
	LightImagedata  	= new (nothrow) unsigned char[width * height];

	tempHue				= new (nothrow) float[width * height];
	tempSaturation		= new (nothrow) float[width * height];
	tempLight			= new (nothrow) float[width * height];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			/*CMY Color Model*/
			red   = float(*(Imagedata + (i * width + j) * BytesPerPixel)) / 255.0;
			green = float(*(Imagedata + (i * width + j) * BytesPerPixel + 1)) / 255.0;
			blue  = float(*(Imagedata + (i * width + j) * BytesPerPixel + 2)) / 255.0;

			*(CyanImagedata    + (i * width) + j) = 255 - *(Imagedata + (i * width + j) * BytesPerPixel);
			*(MagentaImagedata + (i * width) + j) = 255 - *(Imagedata + (i * width + j) * BytesPerPixel + 1);
			*(YellowImagedata  + (i * width) + j) = 255 - *(Imagedata + (i * width + j) * BytesPerPixel + 2);

			/*HSL Color Model*/
			hslM  = max(max(red, green), blue);
			hslm2 = min(min(red, green), blue);
			hslC  = hslM - hslm2;

//			cout << "hslM " << hslM << " hslm2 " << hslm2 << " hslC " << hslC << " width " << j << " height " << i << endl;

			if(hslC == 0) {
				*(tempHue + (i * width) + j) = 0;
				temp1.push_back(*(tempHue + (i * width) + j));
			} else if(hslM == red) {
				*(tempHue + (i * width) + j) = 60 * (int((green -
						blue) / hslC) % 6);
				temp1.push_back(*(tempHue + (i * width) + j));
			} else if(hslM == green) {
				*(tempHue + (i * width) + j) = 60 * ((float(blue - red) / float(hslC)) + 2);
				temp1.push_back(*(tempHue + (i * width) + j));
			} else if(hslM == blue) {
				*(tempHue + (i * width) + j) = 60 * ((float(red - green) / float(hslC)) + 4);
				temp1.push_back(*(tempHue + (i * width) + j));
			}

			*(tempLight + (i * width) + j) = float(hslM + hslm2) / 2.0;
			temp2.push_back(*(tempLight + (i * width) + j));

			if(*(tempLight + (i * width) + j) == 0) {
				*(tempSaturation + (i * width) + j) = 0;
				temp3.push_back(*(tempSaturation + (i * width) + j));
			} else if(0 < *(tempLight + (i * width) + j) and *(tempLight + (i * width) + j) < 0.5) {
				*(tempSaturation + (i * width) + j) = float(hslC) / float(2 * *(tempLight + (i * width) + j));
				temp3.push_back(*(tempSaturation + (i * width) + j));
			} else {
				*(tempSaturation + (i * width) + j) = float(hslC) / float(2.0 - float(2 * *(tempLight + (i * width) + j)));
				temp3.push_back(*(tempSaturation + (i * width) + j));
			}
		}
	}

	sort(temp1.begin(), temp1.end());
	hmin = temp1.front();
	hmax = temp1.back();

	sort(temp2.begin(), temp2.end());
	lmin = temp2.front();
	lmax = temp2.back();

	sort(temp3.begin(), temp3.end());
	smin = temp3.front();
	smax = temp3.back();

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			*(HueImagedata        + (i * width) + j) = char((float(*(tempHue        + (i * width) + j) - hmin) / float(hmax - hmin)) * 255);
			*(SaturationImagedata + (i * width) + j) = char((float(*(tempSaturation + (i * width) + j) - smin) / float(smax - smin)) * 255);
			*(LightImagedata      + (i * width) + j) = char((float(*(tempLight      + (i * width) + j) - lmin) / float(lmax - lmin)) * 255);
		}
	}

	//Write CyanImagedata
	if (!(file=fopen("CyanImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(CyanImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write MagentaImagedata
	if (!(file=fopen("MagentaImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(MagentaImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write YellowImagedata
	if (!(file=fopen("YellowImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(YellowImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write HueImagedata
	if (!(file=fopen("HueImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(HueImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write SaturationImagedata
	if (!(file=fopen("SaturationImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(SaturationImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write LightImagedata
	if (!(file=fopen("LightImage.raw","wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(LightImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	return 0;
}
