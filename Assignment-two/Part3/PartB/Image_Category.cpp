/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016

	File            : This code is for Non-local means noise removal algorithm.
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "Image.h"

using namespace std;

/* Main function*/
int main(int argc, char *argv[]) {

	Image homography;
	int temp1, temp2;

	// Check for proper syntax
	if (argc < 5){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw input_image.raw output_image.raw" << endl;
		return 0;
	}

	homography.allocate_image();
	homography.read_image(argv[1]);
	homography.morphological_processing(0);

	homography.allocate_image();
	homography.read_image(argv[2]);
	homography.morphological_processing(1);

	homography.allocate_image();
	homography.read_image(argv[3]);
	homography.morphological_processing(2);

	cout << "skeletonize 1st " << homography.compare_output[0].skeletonize_count << " thin 1st " <<
			homography.compare_output[0].thin_count << endl;
	cout << "skeletonize 2nd " << homography.compare_output[1].skeletonize_count << " thin 2nd " <<
			homography.compare_output[1].thin_count << endl;
	cout << "skeletonize 3rd " << homography.compare_output[2].skeletonize_count << " thin 3rd " <<
			homography.compare_output[2].thin_count << endl;

	temp1 = abs(homography.compare_output[0].thin_count - homography.compare_output[2].thin_count);
	temp2 = abs(homography.compare_output[1].thin_count - homography.compare_output[2].thin_count);

	if(temp1 < temp2) {
		cout << argv[1] << " and " << argv[3] << " are similar" << endl;
	} else {
		cout << argv[2] << " and " << argv[3] << " are similar" << endl;
	}

	homography.write_image(argv[4], homography.OutputImagedata, homography.width, homography.height);

	return 0;
}
