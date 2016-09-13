// This sample code reads in image data from a RAW image file and
// writes it into another file

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]) {
	// Define file pointer and variables
	FILE *file, *file_output;
	int BytesPerPixel, InpSizeX, InpSizeY;
	int SizeX, SizeY, x1, x2, y1, y2;

	// Check for proper syntax
	if (argc < 10){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel] [SizeX] [SizeY] [X1] [Y1] [X2] [Y2]" << endl;
		return 0;
	} else {
		//Inputs given in the assignment
		BytesPerPixel = atoi(argv[4]);
		InpSizeX 	  = atoi(argv[5]);
		InpSizeY 	  = atoi(argv[6]);
		x1 			  = atoi(argv[7]);
		y1 			  = atoi(argv[8]);
		x2 			  = atoi(argv[9]);
		y2            = atoi(argv[10]);
		printf("Input parameters:\n\tBytes Per Pixel : %d\n\tInpSizeX : %d\n\tInpSizeY : %d\n\tx1 : %d\n\t"
				"y1 : %d\n\tx2 : %d\n\ty2 : %d\n",BytesPerPixel, InpSizeX, InpSizeY, x1, y1, x2, y2);
	}

	SizeX = x2 - x1 + 1;
	SizeY = y2 - y1 + 1;

	// Allocate image data array
	unsigned char *Imagedata;
	unsigned char *CroppedImage;

	Imagedata    = new (nothrow) unsigned char[InpSizeX * InpSizeY * BytesPerPixel];
	CroppedImage = new (nothrow) unsigned char[  SizeX  *   SizeY  * BytesPerPixel];

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

	return 0;
}
