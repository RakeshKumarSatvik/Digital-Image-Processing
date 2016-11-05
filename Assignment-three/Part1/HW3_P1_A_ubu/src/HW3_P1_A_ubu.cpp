/*
    Name            : Rakesh Kumar Satvik
    USC ID          : 8772-6992-51
    USC email       : satvik@usc.edu
    Submission date : 11/06/2016

    File            : This code is for Texture Analysis.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Image.h"

using namespace std;

/* Main function*/
int main(int argc, char *argv[]) {

	Image texture;
	char filename[100];
	int i;

	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw" << endl;
		return 0;
	}

	//Function that is used to allocate the buffers required
	texture.allocate_image();

	for(i = 1; i < 13; i++) {
		texture.image_count = i;
		printf("%d image is being processed\n",i);
		snprintf(filename, sizeof filename, "/home/rakesh/EE569HW3/HW3_P1_A_ubu/Debug/Texture%d.raw",i);
		//Function that is used to read the image from file
		texture.read_image(filename);

		texture.texture_analysis();
		cout << "done with texture analysis" << endl;
		//Function that is used to write into the image file
		texture.write_image(argv[2], texture.OutputImagedata, texture.width, texture.height);
	}

	texture.pca_analysis();
	texture.print_pca_values();
	texture.kmeans_analysis();
	return 0;
}
