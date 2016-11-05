/*
    Name            : Rakesh Kumar Satvik
    USC ID          : 8772-6992-51
    USC email       : satvik@usc.edu
    Submission date : 11/06/2016

	File            : This code is for Texture Analysis.
*/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Image.h"

using namespace cv;
using namespace std;

#define PAD 2

//Constructor
Image::Image() {
	// TODO Auto-generated constructor stub
	this->height 			= 0;
	this->width  			= 0;
	this->BytesPerPixel 	= 1;
	this->Imagedata     	= NULL;
	this->PaddedImagedata   = NULL;
	this->OutputImagedata   = NULL;
	this->coefficients		= new double*[12];
	this->image_count	    = 0;
	this->data              = Mat::zeros(12, 25, CV_32F);
	this->normalizedData    = Mat::zeros(12, 25, CV_32F);
	this->projection_result = Mat::zeros(12, 3, CV_32F);
	this->normalized_projection_result = Mat::zeros(12, 3, CV_32F);

	for(int i = 0; i < 12;i++) {
		this->coefficients[i] = new double[25];
	}
}

//Destructor
Image::~Image() {
	// TODO Auto-generated destructor stub
    delete(this->Imagedata);
    delete(this->PaddedImagedata);
}

/* Function that allocates the buffers required for this program.
 * Takes input from the user for the width, height and number of pixels and
 * creates buffer as per the requirement!
 * */
void Image::allocate_image() {
	cout << "Please enter Bytes Per Pixel : ";
	cin >> this->BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> this->width;
	cout << "Please enter Height of input image: ";
	cin >> this->height;

	// Allocation of image and padded image
	this->Imagedata 	   = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->PaddedImagedata  = new (nothrow) unsigned char[(this->width + 2 * PAD) * (this->height + 2 * PAD) * this->BytesPerPixel];
	this->OutputImagedata  = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
}

/* This function is used to read from the file which is sent as a parameter
 * Input : Filename
 * Output: Content of the file in input buffer
 * */
void Image::read_image(char *filename) {
	// To read/write from/to file
	FILE *file;

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(filename,"rb"))) {
		cout << "Cannot open file: " << filename <<endl;
		exit(1);
	}
	fread(this->Imagedata, sizeof(unsigned char), this->width * this->height * this->BytesPerPixel, file);
	fclose(file);
}

vector<int> Image::calculate_coefficients(int first, int second) {
	vector<int> result;
	int temp;
	int filters[5][5] = {{1, 4, 6, 4, 1},
						{-1, -2, 0, 2, 1},
						{-1, 0, 2, 0, -1},
						{-1, 2, 0, -2, -1},
						{1, -4, 6, -4, 1}};

	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			temp = filters[second][i] * filters[first][j];
			result.push_back(temp);
		}
	}

	return result;
}

/* This function pads the number of rows and columns given by the arguments.
 * */
void Image::padding_image(int padH, int padW) {
	for(int i = 0; i < this->height; i++) {
		//Left padded rows
		memcpy(this->PaddedImagedata + (i + padH) * (this->width + 2 * padW) * this->BytesPerPixel,
				this->Imagedata + (i * this->width) * this->BytesPerPixel,
				padH * this->BytesPerPixel);

		//Main part of the image
		memcpy(this->PaddedImagedata + (i + padH) * (this->width + 2 * padW) * this->BytesPerPixel + padW * this->BytesPerPixel,
				this->Imagedata + (i * this->width) * this->BytesPerPixel,
				this->width * this->BytesPerPixel);

		//Right padded rows
		memcpy( this->PaddedImagedata + ((i + padH + 1) * (this->width + 2 * padW) * this->BytesPerPixel) - padW * this->BytesPerPixel,
				this->PaddedImagedata + ((i + padH + 1) * (this->width + 2 * padW) * this->BytesPerPixel) - 2 * padW * this->BytesPerPixel,
				padW * this->BytesPerPixel);
	}

	for(int i = 0; i < padH;i ++) {
		//Top padded rows
		memcpy(this->PaddedImagedata + i * (this->width + 2 * padW) * this->BytesPerPixel,
				this->PaddedImagedata + padH * (this->width + 2 * padW) * this->BytesPerPixel,
				(this->width + 2 * padW) * this->BytesPerPixel);

		//Bottom padded rows
		memcpy(this->PaddedImagedata + (this->height + padH + i) * (this->width + 2 * padW) * this->BytesPerPixel,
				this->PaddedImagedata + (this->height + padH - 1) * (this->width + 2 * padW) * this->BytesPerPixel,
				(width + 2 * padW) * BytesPerPixel);
	}
}

void Image::apply_mask(vector<int> coefficients) {
	unsigned char *dest;

	this->padding_image(PAD, PAD);
	for(int i = 0; i < this->height; i++) {
		for(int j = 0; j < this->width; j++) {
			dest = this->OutputImagedata + (j + i * this->width) * this->BytesPerPixel;
			for(int l = 0; l < 5; l++) {
				for(int m = 0; m < 5; m++) {
					if (l == m)
						continue;
					*dest += (coefficients.at(l * 5 + m) *
							*(this->OutputImagedata + (j + m + (i + l) * (this->width + 2 * PAD)) * this->BytesPerPixel));
				}
			}
		}
	}
}

double Image::calculate_mean() {
	double mean = 0;

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			mean += *(this->OutputImagedata + (j + i * this->width) * this->BytesPerPixel);
		}
	}

	mean /= (this->height * this->width);

	return mean;
}

void Image::subtract_mean(double mean) {
	for (int i = 0; i < this->height; i++) {
		for(int j = 0; j < this->width; j++) {
			*(this->OutputImagedata + (j + i * this->width) * this->BytesPerPixel) -= mean;
		}
	}
}

double Image::energy_calculator() {
	double energy;
	double mean = this->calculate_mean();
	this->subtract_mean(mean);
	for(int i = 0; i < this->height; i++) {
		for(int j = 0; j < this->width; j++) {
			energy += *(this->OutputImagedata + (j + i * this->width) * this->BytesPerPixel);
		}
	}

	energy *= energy;
	energy /= (this->width * this->height);

	return energy;
}

void Image::print_filters(vector<int> coefficients) {
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			cout << setw(4) << coefficients.at(j + i * 5) << " ";
		}
		cout << endl;
	}
}

void Image::normalize_dump(vector<double> energy) {
	FILE *fp;
	double /*max, min, */normalized;
	cout << "enter this function" << endl;
	if (!(fp = fopen("dump.txt", "a"))) {
		cout << "Cannot open file: " << "dump.txt" << endl;
		exit(1);
	}

//	max = *max_element(energy.begin(), energy.end());
//	min = *min_element(energy.begin(), energy.end());

	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			normalized = energy.at(i * 5 + j);
			fprintf(fp,"%f ",normalized);
			this->coefficients[this->image_count - 1][i * 5 + j] = normalized;
			this->data.at<float>((this->image_count - 1), (i * 5 + j)) = normalized;
		}
	}
	cout << "exit this function" << endl;
	fprintf(fp,"\n");
}

void Image::texture_analysis() {
	vector<int> filter_coefficients;
	vector<double> energy;

	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			filter_coefficients = this->calculate_coefficients(i,j);
//			this->print_filters(filter_coefficients);
			this->apply_mask(filter_coefficients);
			energy.push_back(this->energy_calculator());
			cout << "Energy at " << (i * 5 + j) << " is " << energy.at(i * 5 + j) << endl;
		}
	}
	this->normalize_dump(energy);
}

void Image::print_pca_values() {
	vector<double> columns[25], pca[3];
	double max[25], min[25], pca_max[3], pca_min[3];

	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < 25; j++) {
			printf("%f ",this->coefficients[i][j]);
			columns[j].push_back(this->coefficients[i][j]);
		}
		printf("\n");
	}

	for(int i = 0; i < 25; i++) {
		max[i] = *max_element(columns[i].begin(), columns[i].end());
		min[i] = *min_element(columns[i].begin(), columns[i].end());
	}

	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < 25; j++) {
			this->normalizedData.at<float>(i, j) = (this->data.at<float>(i, j) - min[j]) / (max[j] - min[j]);
		}
	}

	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < 3; j++) {
			pca[j].push_back(this->projection_result.at<float>(i,j));
		}
	}

	for(int i = 0; i < 3; i++) {
		pca_max[i] = *max_element(pca[i].begin(), pca[i].end());
		pca_min[i] = *min_element(pca[i].begin(), pca[i].end());
	}

	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < 3; j++) {
			this->normalized_projection_result.at<float>(i, j) = (this->projection_result.at<float>(i, j) - pca_min[j]) / (pca_max[j] - pca_min[j]);
		}
	}

}

void Image::pca_analysis() {
    int num_components = 3;

//    cout << "Data = "<< endl << " "  << data << endl;
    // Perform a PCA:
    PCA pca(data, Mat(), CV_PCA_DATA_AS_ROW, num_components);

    pca.project(data, this->projection_result);

//    cout<<"PCA Projection Result:"<<endl;
//    cout<<this->projection_result<<endl;
}

void Image::kmeans_analysis() {
	  int clusterCount = 4;
	  Mat labels;
	  int attempts = 15;
	  Mat centers;
	  cout << "For 25-D " << endl;
	  kmeans(normalizedData, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
	  cout << "Labels " << endl << labels << endl;
//	  cout << "centers " << endl << centers << endl;
	  cout << "For 3-D " << endl;
	  kmeans(normalized_projection_result, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
	  cout << "Labels " << endl << labels << endl;
//	  cout << "centers " << endl << centers << endl;
}

/* This function is used to write into the file which is sent as a parameter
 * Input : Filename
 * Output: Content to the file from output buffer
 * */
void Image::write_image(char *filename, unsigned char *data, int inp_width, int inp_height) {
	// To read/write from/to file
	FILE *file;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(data, sizeof(unsigned char), inp_width * inp_height * this->BytesPerPixel, file);
	fclose(file);
}
