// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

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
	int width, height, hslM, hslm2, hslC;
	int hmin, smin, lmin, hmax, smax, lmax;
	vector<int> temp1, temp2, temp3;

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
	unsigned char *Imagedata, *CyanImagedata, *MagentaImagedata, *YellowImagedata;
	unsigned char *HueImagedata, *SaturationImagedata, *LightImagedata;

	Imagedata        	= new (nothrow) unsigned char[width * height * BytesPerPixel];
	CyanImagedata    	= new (nothrow) unsigned char[width * height];
	MagentaImagedata 	= new (nothrow) unsigned char[width * height];
	YellowImagedata  	= new (nothrow) unsigned char[width * height];

	HueImagedata    	= new (nothrow) unsigned char[width * height];
	SaturationImagedata = new (nothrow) unsigned char[width * height];
	LightImagedata  	= new (nothrow) unsigned char[width * height];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen("C:\\Users\\RakeshKumarSatvik\\workspace\\EE569-1-P1\\Debug\\Turtle.raw","rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			/*CMY Color Model*/
			*(CyanImagedata    + (i * width) + j) = 1 - *(Imagedata + (i * width + j) * BytesPerPixel);
			*(MagentaImagedata + (i * width) + j) = 1 - *(Imagedata + (i * width + j) * BytesPerPixel + 1);
			*(YellowImagedata  + (i * width) + j) = 1 - *(Imagedata + (i * width + j) * BytesPerPixel + 2);

			/*HSL Color Model*/
			hslM = max(max(*(Imagedata + (i * width + j) * BytesPerPixel), *(Imagedata + (i * width + j) * BytesPerPixel + 1)),
					*(Imagedata + (i * width + j) * BytesPerPixel + 2));
			hslm2 = min(min(*(Imagedata + (i * width + j) * BytesPerPixel), *(Imagedata + (i * width + j) * BytesPerPixel + 1)),
					*(Imagedata + (i * width + j) * BytesPerPixel + 2));
			hslC = hslM - hslm2;

			cout << "hslM " << hslM << " hslm2 " << hslm2 << " hslC " << hslC << " width " << j << " height " << i << endl;

			if(hslC == 0) {
				*(HueImagedata + (i * width) + j) = 0;
				temp1.push_back(*(HueImagedata + (i * width) + j));
			} else if(hslM == *(Imagedata + (i * width + j) * BytesPerPixel)) {
				*(HueImagedata + (i * width) + j) = 60 * (((*(Imagedata + (i * width + j) * BytesPerPixel + 1) -
						*(Imagedata + (i * width + j) * BytesPerPixel + 2)) / hslC) % 6);
				temp1.push_back(*(HueImagedata + (i * width) + j));
			} else if(hslM == *(Imagedata + (i * width + j) * BytesPerPixel + 1)) {
				*(HueImagedata + (i * width) + j) = 60 * (((*(Imagedata + (i * width + j) * BytesPerPixel + 2) -
						*(Imagedata + (i * width + j) * BytesPerPixel)) / hslC) + 2);
				temp1.push_back(*(HueImagedata + (i * width) + j));
			} else if(hslM == *(Imagedata + (i * width + j) * BytesPerPixel + 2)) {
				*(HueImagedata + (i * width) + j) = 60 * (((*(Imagedata + (i * width + j) * BytesPerPixel) -
						*(Imagedata + (i * width + j) * BytesPerPixel + 1)) / hslC) + 4);
				temp1.push_back(*(HueImagedata + (i * width) + j));
			}

			*(LightImagedata + (i * width) + j) = (hslM + hslm2) / 2;
			temp2.push_back(*(LightImagedata + (i * width) + j));

			if(*(LightImagedata + (i * width) + j) == 0) {
				*(SaturationImagedata + (i * width) + j) = 0;
				temp3.push_back(*(SaturationImagedata + (i * width) + j));
			} else if(0 < *(LightImagedata + (i * width) + j) and *(LightImagedata + (i * width) + j) < 0.5) {
				*(SaturationImagedata + (i * width) + j) = hslC / 2 * *(LightImagedata + (i * width) + j);
				temp3.push_back(*(SaturationImagedata + (i * width) + j));
			} else {
				if (*(LightImagedata + (i * width) + j) != 1)
					*(SaturationImagedata + (i * width) + j) = hslC / (2 - 2 * *(LightImagedata + (i * width) + j));
				else
					*(SaturationImagedata + (i * width) + j) = hslC / (2 - *(LightImagedata + (i * width) + j));
				temp3.push_back(*(SaturationImagedata + (i * width) + j));
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

	cout << "hmin : " << hmin << " hmax : " << hmax << " smax : " << smax << " smin : " << smin << " lmax : " << lmax << " lmin : " << lmin << endl;
	cout << "Reached here" << endl;

//	for(int i = 0; i < height; i++) {
//		for(int j = 0; j < width; j++) {
//			*(HueImagedata        + (i * width) + j) = ((*(HueImagedata        + (i * width) + j) - hmin) / (hmax - hmin)) * 255;
//			*(SaturationImagedata + (i * width) + j) = ((*(SaturationImagedata + (i * width) + j) - smin) / (smax - smin)) * 255;
//			*(LightImagedata      + (i * width) + j) = ((*(LightImagedata      + (i * width) + j) - lmin) / (lmax - lmin)) * 255;
//		}
//	}

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), width * height * BytesPerPixel, file);
	fclose(file);

	//Write CyanImagedata
	if (!(file=fopen(argv[3],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(CyanImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write MagentaImagedata
	if (!(file=fopen(argv[4],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(MagentaImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write YellowImagedata
	if (!(file=fopen(argv[5],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(YellowImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write HueImagedata
	if (!(file=fopen(argv[6],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(HueImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write SaturationImagedata
	if (!(file=fopen(argv[7],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(SaturationImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	//Write LightImagedata
	if (!(file=fopen(argv[8],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(LightImagedata, sizeof(unsigned char), width * height, file);
	fclose(file);

	return 0;
}
