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

	Image rotation;

	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw" << endl;
		return 0;
	}

	rotation.allocate_image();
	rotation.read_image(argv[1]);

	rotation.image_rotation(20);
	rotation.write_image(argv[2]);

	return 0;
}
