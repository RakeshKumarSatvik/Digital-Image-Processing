// This sample code reads in image data from a RAW image file and
// writes it into another file

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]) {
	// Define file pointer and variables
	FILE *file, *file_output, *file_enhanced;
	int BytesPerPixel, InpSizeX, InpSizeY;
	int SizeX, SizeY, x1, x2, y1, y2;
	int EnhX, EnhY;
	float pPrime, qPrime;

	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw" << endl;
		return 0;
	}

	//Inputs given in the assignment
	cout << "Bytes Per Pixel : ";
	cin >> BytesPerPixel;
	cout << "InpSizeX : ";
	cin >> InpSizeX;
	cout << "InpSizeY : ";
	cin >> InpSizeY;
	cout << "X1 co-ordinate : ";
	cin >> x1;
	cout << "Y1 co-ordinate : ";
	cin >> y1;
	cout << "X2 co-ordinate : ";
	cin >> x2;
	cout << "Y1 co-ordinate : ";
	cin >> y2;
	cout << "Width of the enhanced image : ";
	cin >> EnhX;
	cout << "Height of the enhanced image : ";
	cin >> EnhY;

	printf("Input parameters:\n\tBytes Per Pixel : %d\n\tInpSizeX : %d\n\tInpSizeY : %d\n\tx1 : %d\n\t"
			"y1 : %d\n\tx2 : %d\n\ty2 : %d\n",BytesPerPixel, InpSizeX, InpSizeY, x1, y1, x2, y2);

	SizeX = x2 - x1 + 1;
	SizeY = y2 - y1 + 1;

	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *CroppedImage;
	unsigned char *EnhancedImage;

	Imagedata     = new (nothrow) unsigned char[InpSizeX * InpSizeY * BytesPerPixel];
	CroppedImage  = new (nothrow) unsigned char[  SizeX  *   SizeY  * BytesPerPixel];
	EnhancedImage = new (nothrow) unsigned char[  EnhX   *    EnhY  * BytesPerPixel];

	//Initialize to zeros
	std::fill_n(CroppedImage, sizeof(unsigned char) * SizeX * SizeY * BytesPerPixel, 0);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	if(fread(Imagedata, sizeof(unsigned char), InpSizeX * InpSizeY * BytesPerPixel, file) < 0) {
		cout << "Error in reading file" << endl;
	}
	fclose(file);

	printf("Size of cropped image : %dX%d\n",SizeX, SizeY);

	for(int i = 0; i < SizeY; i++) {
		memcpy(CroppedImage
					+ i * SizeX * BytesPerPixel, //Move the cursor in the output buffer
				Imagedata
					+ (x1 - 1) * BytesPerPixel + (y1 - 1) * InpSizeX * BytesPerPixel //Take to the first line
					+ i * InpSizeX * BytesPerPixel, //Move to the specific line as per the loop
				SizeX * BytesPerPixel); //Size copied per each line
	}

	for(int j = 0; j < EnhY; j++) {
		for(int k = 0; k < EnhX; k++) {
			pPrime = 1 + (float(SizeX - 1) / float(EnhX - 1)) * (k - 1);
			qPrime = 1 + (float(SizeY - 1) / float(EnhY - 1)) * (j - 1);

//			printf("%lf,%lf ", pPrime, qPrime);

			if(pPrime - int(pPrime) != 0 || qPrime - int(qPrime) != 0) {
				/*Bilinear Implementation*/
				int BIx, BIy;
				int numerator1, numerator2, numerator;
				BIx = int(pPrime);
				BIy = int(qPrime);

				//Loop for the 24bit RBG values
				for(int i = 0; i < 3; i++) {
					numerator1 = ((BIx + 1 - pPrime) * *(CroppedImage + ((BIx + BIy * SizeX) * BytesPerPixel) + i) +
										(pPrime - BIx) * *(CroppedImage + (((BIx + 1) + BIy * SizeX) * BytesPerPixel) + i)) *
												(BIy + 1 - qPrime);

					numerator2 = ((BIx + 1 - pPrime) * *(CroppedImage + ((BIx + (BIy + 1) * SizeX) * BytesPerPixel) + i) +
									(pPrime - BIx) * *(CroppedImage + (((BIx + 1 )+ (BIy + 1) * SizeX) * BytesPerPixel) + i)) *
												(qPrime - BIy);

					numerator   = numerator1 + numerator2;
					*(EnhancedImage + (k + j * EnhX) * BytesPerPixel + i) = numerator;
				}
			} else {
				//else if the points are integer values, then directly copy from the cropped image
				memcpy(EnhancedImage + (k + j * EnhX) * BytesPerPixel, CroppedImage + (int(pPrime) + int(qPrime) * SizeX) * BytesPerPixel,
						BytesPerPixel);
			}
		}
//		printf("\n");
	}

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), InpSizeX * InpSizeY * BytesPerPixel, file);
	fclose(file);

	// Write cropped image data
	if (!(file_output=fopen(argv[3] ,"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(CroppedImage, sizeof(unsigned char), SizeX * SizeY * BytesPerPixel, file_output);
	fclose(file_output);

	// Write cropped image data
	if (!(file_enhanced=fopen(argv[4] ,"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(EnhancedImage, sizeof(unsigned char), EnhX * EnhY * BytesPerPixel, file_enhanced);
	fclose(file_enhanced);
	return 0;
}
