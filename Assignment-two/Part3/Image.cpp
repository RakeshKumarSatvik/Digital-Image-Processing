/*
	Name            : Rakesh Kumar Satvik
	USC ID          : 8772-6992-51
	USC email       : satvik@usc.edu
	Submission date : 18/09/2016

	File            : This code is for Non-local means noise removal algorithm.
*/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include "Image.h"
#include <string.h>
#include <stdlib.h>

#include "tables.h"

using namespace std;

int padH = 10/2;
int padW = 10/2;

Image::Image() {
	// TODO Auto-generated constructor stub
	this->height              = 0;
	this->width               = 0;
	this->BytesPerPixel       = 1;
	this->Imagedata           = NULL;
	this->OutputImagedata     = NULL;
	this->PaddedImagedata     = NULL;
	this->BinaryImagedata     = NULL;
	this->HitorMissImagedata  = NULL;
	this->TempImagedata     = NULL;
}

Image::~Image() {
	// TODO Auto-generated destructor stub
	delete(this->Imagedata);
}

void Image::allocate_image() {

	cout << "Please enter Bytes Per Pixel : ";
	cin >> this->BytesPerPixel;
	cout << "Please enter Width of input image: ";
	cin >> this->width;
	cout << "Please enter Height of input image: ";
	cin >> this->height;

	// Allocation of image and padded image
	this->Imagedata 	      = new (nothrow) unsigned char[this->width * this->height * this->BytesPerPixel];
	this->OutputImagedata     = new (nothrow) unsigned char[this->width * this->height];
	this->BinaryImagedata     = new (nothrow) unsigned char[this->width * this->height];
	this->TempImagedata       = new (nothrow) unsigned char[this->width * this->height];
	this->HitorMissImagedata  = new (nothrow) unsigned char[this->width * this->height];
	this->PaddedImagedata     = new (nothrow) unsigned char[(this->width + 2 * padH) * (this->height + 2 * padW)];

	memset(this->HitorMissImagedata , 0, this->width * this->height);
	memset(this->TempImagedata      , 0, this->width * this->height);
}

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

void Image::image_grayscale() {
    int red, green, blue;

    for(int i = 0; i < this->height; i++) {
        for(int j = 0; j < this->width; j++) {
            red   = int(*(this->Imagedata + (j + i * this->width) * this->BytesPerPixel + 1));
            green = int(*(this->Imagedata + (j + i * this->width) * this->BytesPerPixel + 2));
            blue  = int(*(this->Imagedata + (j + i * this->width) * this->BytesPerPixel + 3));

            *(this->OutputImagedata + j + i * this->width) = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
        }
    }
}

void Image::padding_image(unsigned char *data) {
    for(int i = 0; i < this->height; i++) {
        //Left padded rows
        for(int j = 0; j < padW; j++) {
            memcpy(this->PaddedImagedata + j + (i + padH) * (this->width + 2 * padW),
                    data + (i * this->width),
                    1);
        }

        //Main part of the image
        memcpy(this->PaddedImagedata + (i + padH) * (this->width + 2 * padW) + padW,
                data + (i * this->width),
                this->width);

        //Right padded rows
        for(int j = 0; j < padW; j++) {
            memcpy( this->PaddedImagedata + (j + (i + padH + 1) * (this->width + 2 * padW)) - padW,
                    this->PaddedImagedata + ((i + padH + 1) * (this->width + 2 * padW)) - 1 - padW,
                    1);
        }
    }

    for(int i = 0; i < padH;i ++) {
        //Top padded rows
        memcpy(this->PaddedImagedata + i * (this->width + 2 * padW),
                this->PaddedImagedata + padH * (this->width + 2 * padW),
                (this->width + 2 * padW));

        //Bottom padded rows
        memcpy(this->PaddedImagedata + (this->height + padH + i) * (this->width + 2 * padW),
                this->PaddedImagedata + (this->height + padH - 1) * (this->width + 2 * padW),
                (width + 2 * padW));
    }
}

void Image::image_binarization() {
    int mean, temp_i, temp_j;

    for(int i = padH; i < this->height + padH; i++) {
        for(int j = padW; j < this->width + padW; j++) {
            mean = 0;
            temp_i = i - padH;
            temp_j = j - padW;
            for(int m = 0; m < padH; m++) {
                for(int n = 0; n < padW; n++) {
                    mean += int(*(this->PaddedImagedata + m + temp_j + (n + temp_i) * (this->width + 2 * padW)));
                }
            }
            mean /= (padH * padW);
//            cout << mean << endl;
            if(mean > int(*(this->PaddedImagedata + j + i * (this->width + 2 * padW)))) {
                *(this->BinaryImagedata + temp_j + temp_i * this->width) = 255;
            } else {
                *(this->BinaryImagedata + temp_j + temp_i * this->width) = 0;
            }
        }
    }
}

void Image::generate_table() {
    this->first_table = {
    		{0, 0, 1, 0, 1, 0, 0, 0, 0}, //s_bond_one_1
            {1, 0, 0, 0, 1, 0, 0, 0, 0}, //s_bond_one_2
            {0, 0, 0, 0, 1, 0, 1, 0, 0}, //s_bond_one_3
            {0, 0, 0, 0, 1, 0, 0, 0, 1}, //s_bond_one_4
            {0, 0, 0, 0, 1, 1, 0, 0, 0}, //s_bond_two_1
            {0, 1, 0, 0, 1, 0, 0, 0, 0}, //s_bond_two_2
            {0, 0, 0, 1, 1, 0, 0, 0, 0}, //s_bond_two_3
            {0, 0, 0, 0, 1, 0, 0, 1, 0}, //s_bond_two_4
            {0, 0, 1, 0, 1, 1, 0, 0, 0}, //s_bond_three_1
            {0, 1, 1, 0, 1, 0, 0, 0, 0}, //s_bond_three_2
            {1, 1, 0, 0, 1, 0, 0, 0, 0}, //s_bond_three_3
            {1, 0, 0, 1, 1, 0, 0, 0, 0}, //s_bond_three_4
            {0, 0, 0, 1, 1, 0, 1, 0, 0}, //s_bond_three_5
            {0, 0, 0, 0, 1, 0, 1, 1, 0}, //s_bond_three_6
            {0, 0, 0, 0, 1, 0, 0, 1, 1}, //s_bond_three_7
            {0, 0, 0, 0, 1, 1, 0, 0, 1}, //s_bond_three_8
            {0, 1, 0, 0, 1, 1, 0, 0, 0}, //tk_bond_four_1
            {0, 1, 0, 1, 1, 0, 0, 0, 0}, //tk_bond_four_2
            {0, 0, 0, 1, 1, 0, 0, 1, 0}, //tk_bond_four_3
            {0, 0, 0, 0, 1, 1, 0, 1, 0}, //tk_bond_four_4
            {0, 0, 1, 0, 1, 1, 0, 0, 1}, //stk_bond_four_1
            {1, 1, 1, 0, 1, 0, 0, 0, 0}, //stk_bond_four_2
            {1, 0, 0, 1, 1, 0, 1, 0, 0}, //stk_bond_four_3
            {0, 0, 0, 0, 1, 0, 1, 1, 1}, //stk_bond_four_4
            {1, 1, 0, 0, 1, 1, 0, 0, 0}, //st_bond_five_1
            {0, 1, 0, 0, 1, 1, 0, 0, 1}, //st_bond_five_2
            {0, 1, 1, 1, 1, 0, 0, 0, 0}, //st_bond_five_3
            {0, 0, 1, 0, 1, 1, 0, 1, 0}, //st_bond_five_4
            {0, 1, 1, 0, 1, 1, 0, 0, 0}, //st_bond_five_5
            {1, 1, 0, 1, 1, 0, 0, 0, 0}, //st_bond_five_6
            {0, 0, 0, 1, 1, 0, 1, 1, 0}, //st_bond_five_7
            {0, 0, 0, 0, 1, 1, 0, 1, 1}, //st_bond_five_8
            {1, 1, 0, 0, 1, 1, 0, 0, 1}, //st_bond_six_1
            {0, 1, 1, 1, 1, 0, 1, 0, 0}, //st_bond_six_2
            {1, 1, 1, 0, 1, 1, 0, 0, 0}, //stk_bond_six_1
            {0, 1, 1, 0, 1, 1, 0, 0, 1}, //stk_bond_six_2
            {1, 1, 1, 1, 1, 0, 0, 0, 0}, //stk_bond_six_3
            {1, 1, 0, 1, 1, 0, 1, 0, 0}, //stk_bond_six_4
            {1, 0, 0, 1, 1, 0, 1, 1, 0}, //stk_bond_six_5
            {0, 0, 0, 1, 1, 0, 1, 1, 1}, //stk_bond_six_6
            {0, 0, 0, 0, 1, 1, 1, 1, 1}, //stk_bond_six_7
            {0, 0, 1, 0, 1, 1, 0, 1, 1}, //stk_bond_six_8
            {1, 1, 1, 0, 1, 1, 0, 0, 1}, //stk_bond_seven_1
            {1, 1, 1, 1, 1, 0, 1, 0, 0}, //stk_bond_seven_2
            {1, 0, 0, 1, 1, 0, 1, 1, 1}, //stk_bond_seven_3
            {0, 0, 1, 0, 1, 1, 1, 1, 1}, //stk_bond_seven_4
            {0, 1, 1, 0, 1, 1, 0, 1, 1}, //stk_bond_eight_1
            {1, 1, 1, 1, 1, 1, 0, 0, 0}, //stk_bond_eight_2
            {1, 1, 0, 1, 1, 0, 1, 1, 0}, //stk_bond_eight_3
            {0, 0, 0, 1, 1, 1, 1, 1, 1}, //stk_bond_eight_4
            {1, 1, 1, 0, 1, 1, 0, 1, 1}, //stk_bond_nine_1
            {0, 1, 1, 0, 1, 1, 1, 1, 1}, //stk_bond_nine_2
            {1, 1, 1, 1, 1, 1, 1, 0, 0}, //stk_bond_nine_3
            {1, 1, 1, 1, 1, 1, 0, 0, 1}, //stk_bond_nine_4
            {1, 1, 1, 1, 1, 0, 1, 1, 0}, //stk_bond_nine_5
            {1, 1, 0, 1, 1, 0, 1, 1, 1}, //stk_bond_nine_6
            {1, 0, 0, 1, 1, 1, 1, 1, 1}, //stk_bond_nine_7
            {0, 0, 1, 1, 1, 1, 1, 1, 1}, //stk_bond_nine_8
            {1, 1, 1, 0, 1, 1, 1, 1, 1}, //stk_bond_ten_1
            {1, 1, 1, 1, 1, 1, 1, 0, 1}, //stk_bond_ten_2
            {1, 1, 1, 1, 1, 0, 1, 1, 1}, //stk_bond_ten_3
            {1, 0, 1, 1, 1, 1, 1, 1, 1}, //stk_bond_ten_4
            {1, 1, 1, 1, 1, 1, 0, 1, 1}, //stk_bond_eleven_1
            {1, 1, 1, 1, 1, 1, 1, 1, 0}, //stk_bond_eleven_2
            {1, 1, 0, 1, 1, 1, 1, 1, 1}, //stk_bond_eleven_3
            {0, 1, 1, 1, 1, 1, 1, 1, 1}, //stk_bond_eleven_4
    };

    this->second_table = {
    		{0, 0, 1, 0, 1, 0, 0, 0, 0}, //spur_1
    		{1, 0, 0, 0, 1, 0, 0, 0, 0}, //spur_2
    		{0, 0, 0, 0, 1, 0, 0, 1, 0}, //single_4_connection_1
    		{0, 0, 0, 0, 1, 1, 0, 0, 0}, //single_4_connection_2
    		{0, 0, 1, 0, 1, 1, 0, 0, 0}, //L_cluster_1
    		{0, 1, 1, 0, 1, 0, 0, 0, 0}, //L_cluster_2
    		{1, 1, 0, 0, 1, 0, 0, 0, 0}, //L_cluster_3
    		{1, 0, 0, 1, 1, 0, 0, 0, 0}, //L_cluster_4
    		{0, 0, 0, 1, 1, 0, 1, 0, 0}, //L_cluster_5
    		{0, 0, 0, 0, 1, 0, 1, 1, 0}, //L_cluster_6
    		{0, 0, 0, 0, 1, 0, 0, 1, 1}, //L_cluster_7
    		{0, 0, 0, 0, 1, 1, 0, 0, 1}, //L_cluster_8
    		{0, 1, 1, 1, 1, 0, 0, 0, 0}, //4_connected_offset_1
    		{1, 1, 0, 0, 1, 1, 0, 0, 0}, //4_connected_offset_2
    		{0, 1, 0, 0, 1, 1, 0, 0, 1}, //4_connected_offset_3
    		{0, 0, 1, 0, 1, 1, 0, 1, 0}, //4_connected_offset_4
    		{0, 1, 1, 0, 1, 0, 1, 0, 0}, //Spur_corner_cluster_1_AB_10
    		{0, 0, 1, 0, 1, 1, 1, 0, 0}, //Spur_corner_cluster_1_AB_01
    		{0, 1, 1, 0, 1, 1, 1, 0, 0}, //Spur_corner_cluster_1_AB_11
    		{1, 0, 0, 1, 1, 0, 0, 0, 1}, //Spur_corner_cluster_2_AB_10
    		{1, 1, 0, 0, 1, 0, 0, 0, 1}, //Spur_corner_cluster_2_AB_01
    		{1, 1, 0, 1, 1, 0, 0, 0, 1}, //Spur_corner_cluster_2_AB_11
    		{0, 0, 1, 1, 1, 0, 1, 0, 0}, //Spur_corner_cluster_3_AB_10
    		{0, 0, 1, 0, 1, 0, 1, 1, 0}, //Spur_corner_cluster_3_AB_01
    		{0, 0, 1, 1, 1, 0, 1, 1, 0}, //Spur_corner_cluster_3_AB_01
    		{1, 0, 0, 0, 1, 0, 0, 1, 1}, //Spur_corner_cluster_4_AB_10
    		{1, 0, 0, 0, 1, 1, 0, 0, 1}, //Spur_corner_cluster_4_AB_01
    		{1, 0, 0, 0, 1, 1, 0, 1, 1}, //Spur_corner_cluster_4_AB_11
    		{1, 1, 2, 1, 1, 2, 2, 2, 2}, //Corner_cluster_1_D_0
    		{2, 1, 0, 1, 1, 1, 2, 0, 0}, //Tee_Branch_1_DD_00
    		{0, 1, 2, 1, 1, 1, 0, 0, 2}, //Tee_Branch_2_DD_00
    		{0, 0, 2, 1, 1, 1, 0, 1, 2}, //Tee_Branch_3_DD_00
    		{2, 0, 0, 1, 1, 1, 2, 1, 0}, //Tee_Branch_4_DD_00
    		{2, 1, 2, 1, 1, 0, 0, 1, 0}, //Tee_Branch_5_DD_00
    		{0, 1, 0, 1, 1, 0, 2, 1, 2}, //Tee_Branch_6_DD_00
    		{0, 1, 0, 0, 1, 1, 2, 1, 2}, //Tee_Branch_7_DD_00
    		{2, 1, 2, 0, 1, 1, 0, 1, 0}, //Tee_Branch_8_DD_00
    		{1, 2, 1, 2, 1, 2, 0, 0, 1}, //Vee_Branch_1_DDD_000_ABC_001
    		{1, 2, 1, 2, 1, 2, 0, 1, 0}, //Vee_Branch_1_DDD_000_ABC_010
    		{1, 2, 1, 2, 1, 2, 0, 1, 1}, //Vee_Branch_1_DDD_000_ABC_011
    		{1, 2, 1, 2, 1, 2, 1, 0, 0}, //Vee_Branch_1_DDD_000_ABC_100
    		{1, 2, 1, 2, 1, 2, 1, 0, 1}, //Vee_Branch_1_DDD_000_ABC_101
    		{1, 2, 1, 2, 1, 2, 1, 1, 0}, //Vee_Branch_1_DDD_000_ABC_110
    		{1, 2, 1, 2, 1, 2, 1, 1, 1}, //Vee_Branch_1_DDD_000_ABC_111
    		{1, 2, 0, 2, 1, 0, 1, 2, 1}, //Vee_Branch_2_DDD_000_ABC_001
    		{1, 2, 0, 2, 1, 1, 1, 2, 0}, //Vee_Branch_2_DDD_000_ABC_010
    		{1, 2, 0, 2, 1, 1, 1, 2, 1}, //Vee_Branch_2_DDD_000_ABC_011
    		{1, 2, 1, 2, 1, 0, 1, 2, 0}, //Vee_Branch_2_DDD_000_ABC_100
    		{1, 2, 1, 2, 1, 0, 1, 2, 1}, //Vee_Branch_2_DDD_000_ABC_101
    		{1, 2, 1, 2, 1, 1, 1, 2, 0}, //Vee_Branch_2_DDD_000_ABC_110
    		{1, 2, 1, 2, 1, 1, 1, 2, 1}, //Vee_Branch_2_DDD_000_ABC_111
    		{0, 0, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_001
    		{0, 1, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_010
    		{0, 1, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_011
    		{1, 0, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_100
    		{1, 0, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_101
    		{1, 1, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_110
    		{1, 1, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_DDD_000_ABC_111
    		{0, 2, 1, 0, 1, 2, 1, 2, 1}, //Vee_Branch_4_DDD_000_ABC_001
    		{0, 2, 1, 1, 1, 2, 0, 2, 1}, //Vee_Branch_4_DDD_000_ABC_010
    		{0, 2, 1, 1, 1, 2, 1, 2, 1}, //Vee_Branch_4_DDD_000_ABC_011
    		{1, 2, 1, 0, 1, 2, 0, 2, 1}, //Vee_Branch_4_DDD_000_ABC_100
    		{1, 2, 1, 0, 1, 2, 1, 2, 1}, //Vee_Branch_4_DDD_000_ABC_101
    		{1, 2, 1, 1, 1, 2, 0, 2, 1}, //Vee_Branch_4_DDD_000_ABC_110
    		{1, 2, 1, 1, 1, 2, 1, 2, 1}, //Vee_Branch_4_DDD_000_ABC_111
    		{2, 1, 0, 0, 1, 1, 1, 0, 2}, //Diagonal_Branch_1
    		{0, 1, 2, 1, 1, 0, 2, 0, 1}, //Diagonal_Branch_2
    		{2, 0, 1, 1, 1, 0, 0, 1, 2}, //Diagonal_Branch_3
    		{1, 0, 2, 0, 1, 1, 2, 1, 0}, //Diagonal_Branch_4
    };

    this->third_table= {
    		{0, 0, 0, 0, 1, 0, 0, 0 ,1}, //Spur_1
    		{0, 0, 0, 0, 1, 0, 1, 0, 0}, //Spur_2
    		{0, 0, 1, 0, 1, 0, 0, 0, 0}, //Spur_3
    		{1, 0, 0, 0, 1, 0, 0, 0, 0}, //Spur_4
    		{0, 0, 0, 0, 1, 0, 0, 1, 0}, //Single_4_connection_1
    		{0, 0, 0, 0, 1, 1, 0, 0, 0}, //Single_4_connection_2
    		{0, 0, 0, 1, 1, 0, 0, 0, 0}, //Single_4_connection_3
    		{0, 1, 0, 0, 1, 0, 0, 0, 0}, //Single_4_connection_4
    		{0, 1, 0, 0, 1, 1, 0, 0, 0}, //L_corner_1
    		{0, 1, 0, 1, 1, 0, 0, 0, 0}, //L_corner_2
    		{0, 0, 0, 0, 1, 1, 0, 1, 0}, //L_corner_3
    		{0, 0, 0, 1, 1, 0, 0, 1, 0}, //L_corner_4
    		{1, 1, 2, 1, 1, 2, 2, 2, 2}, //Corner_cluster_1
    		{2, 2, 2, 2, 1, 1, 2, 1, 1}, //Corner_cluster_2
    		{2, 1, 2, 1, 1, 1, 2, 2, 2}, //Tee_Branch_1
    		{2, 1, 2, 1, 1, 2, 2, 1, 2}, //Tee_Branch_2
    		{2, 2, 2, 1, 1, 1, 2, 1, 2}, //Tee_Branch_3
    		{2, 1, 2, 2, 1, 1, 2, 1, 2}, //Tee_Branch_4
    		{1, 2, 1, 2, 1, 2, 0, 0, 1}, //Vee_Branch_1_ABC_001
    		{1, 2, 1, 2, 1, 2, 0, 1, 0}, //Vee_Branch_1_ABC_010
    		{1, 2, 1, 2, 1, 2, 0, 1, 1}, //Vee_Branch_1_ABC_011
    		{1, 2, 1, 2, 1, 2, 1, 0, 0}, //Vee_Branch_1_ABC_100
    		{1, 2, 1, 2, 1, 2, 1, 0, 1}, //Vee_Branch_1_ABC_101
    		{1, 2, 1, 2, 1, 2, 1, 1, 0}, //Vee_Branch_1_ABC_110
    		{1, 2, 1, 2, 1, 2, 1, 1, 1}, //Vee_Branch_1_ABC_111
    		{1, 2, 0, 2, 1, 0, 1, 2, 1}, //Vee_Branch_2_ABC_001
    		{1, 2, 0, 2, 1, 1, 1, 2, 0}, //Vee_Branch_2_ABC_010
    		{1, 2, 0, 2, 1, 1, 1, 2, 1}, //Vee_Branch_2_ABC_011
    		{1, 2, 1, 2, 1, 0, 1, 2, 0}, //Vee_Branch_2_ABC_100
    		{1, 2, 1, 2, 1, 0, 1, 2, 1}, //Vee_Branch_2_ABC_101
    		{1, 2, 1, 2, 1, 1, 1, 2, 0}, //Vee_Branch_2_ABC_110
    		{1, 2, 1, 2, 1, 1, 1, 2, 1}, //Vee_Branch_2_ABC_110
    		{0, 0, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_001
    		{0, 1, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_010
    		{0, 1, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_011
    		{1, 0, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_100
    		{1, 0, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_101
    		{1, 1, 0, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_110
    		{1, 1, 1, 2, 1, 2, 1, 2, 1}, //Vee_Branch_3_ABC_111
    		{0, 2, 1, 0, 1, 2, 1, 2, 1}, //Vee_Branch_4_ABC_001
    		{0, 2, 1, 1, 1, 2, 0, 2, 1}, //Vee_Branch_4_ABC_010
    		{0, 2, 1, 1, 1, 2, 1, 2, 1}, //Vee_Branch_4_ABC_011
    		{1, 2, 1, 0, 1, 2, 0, 2, 1}, //Vee_Branch_4_ABC_100
    		{1, 2, 1, 0, 1, 2, 1, 2, 1}, //Vee_Branch_4_ABC_101
    		{1, 2, 1, 1, 1, 2, 0, 2, 1}, //Vee_Branch_4_ABC_110
    		{1, 2, 1, 1, 1, 2, 1, 2, 1}, //Vee_Branch_4_ABC_111
    		{2, 1, 0, 0, 1, 1, 1, 0, 2}, //Diagonal_branch_1
    		{0, 1, 2, 1, 1, 0, 2, 0, 1}, //Diagonal_branch_2
    		{2, 0, 1, 1, 1, 0, 0, 1, 2}, //Diagonal_branch_3
    		{1, 0, 2, 0, 1, 1, 2, 1, 0}, //Diagonal_branch_4
    };
}

void Image::image_shrinking() {
	int sum = 0, temp_j, temp_i;
	vector<int> table;

	for (int i = 0;i < this->height; i++) {
		for (int j = 0;j < this->width; j++) {
			*(this->HitorMissImagedata + j + i * this->width) = (int(*(this->BinaryImagedata + j + i * this->width)))/255;
		}
	}
	for(int l=0;l<40;l++)    // iteration times
	{
		memset(this->TempImagedata, 0, this->width * this->height);

		for (int i=1;i<this->height-1;i++) {
			for (int j=1;j<this->width-1;j++) {
				temp_j = j - 1;
				temp_i = i - 1;

				if (int(*(this->HitorMissImagedata + j + i * this->width)) == 1) {
					for(int mat = 0; mat < this->first_table.size(); mat++) {
						table = this->first_table.at(mat);
						sum = 0;
						/*Apply the mask - first stage*/
						for(int m = 0; m < 3; m++) {
							for(int n = 0; n < 3; n++) {
								if(table.at(n + m * 3) != 2) {
									if(int(*(this->HitorMissImagedata + temp_j + n + (temp_i + m) * this->width)) == table.at(n + m * 3)) {
										sum++;
									}
								}
							}
						}
						if(sum == 9) {
							*(this->TempImagedata + j + i * this->width) = 1;
						}
					}
				}
			}
		}

		for (int i = 1; i < this->height - 1; i++) {
			for (int j = 1; j < this->width - 1; j++) {
				temp_j = j - 1;
				temp_i = i - 1;

				if (int(*(this->TempImagedata + j + i * this->width)) == 1)
				{
					for(int mat = 0; mat < this->second_table.size(); mat++) {
						sum = 0;
						table = this->second_table.at(mat);
						/*Apply the mask - second stage*/
						for(int m = 0; m < 3; m++) {
							for(int n = 0; n < 3; n++) {
								if(table.at(n + m * 3) != 2) {
									if(int(*(this->TempImagedata + j + n + (i + m) * this->width)) == table.at(n + m * 3)) {
										sum++;
									}
								} else {
									sum++;
								}
							}
						}
						if(sum == 9) {
							*(this->HitorMissImagedata + j + i * this->width) = 1;
							break;
						} else {
							*(this->HitorMissImagedata + j + i * this->width) = 0;
						}
					}
				}
			}
		}
	}

	sum = 0;
	for (int i = 0; i < this->height; i++) {
		for (int j = 0; j < this->width; j++) {
			if (int(*(this->HitorMissImagedata + j + i * this->width)) == 1) {
				sum++;
				*(this->OutputImagedata + j + i * this->width) = 255;
			}
			else {
				*(this->OutputImagedata + j + i * this->width) = 0;
			}
		}
	}
	cout << sum << endl;
}

void Image::print_table() {
	vector<int> table;

	cout << endl << endl;

	for(int mat = 0; mat < this->first_table.size(); mat++) {
		table = this->first_table.at(mat);
		for(int m = 0; m < 3; m++) {
			for(int n = 0; n < 3; n++) {
				cout << table.at(n + m * 3) << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}

	cout << "Second stage matrix" << endl << endl;

	for(int mat = 0; mat < this->second_table.size(); mat++) {
		table = this->second_table.at(mat);
		for(int m = 0; m < 3; m++) {
			for(int n = 0; n < 3; n++) {
				cout << table.at(n + m * 3) << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
}

void Image::homographical_processing() {
    this->image_grayscale();
    this->padding_image(this->OutputImagedata);
    this->image_binarization();
    this->write_image("/home/rakesh/workspace/ee569/Debug/Binary.raw", this->BinaryImagedata, this->width, this->height);
    this->generate_table();
//    this->print_table();
    this->image_shrinking();
}

void Image::write_image(char *filename, unsigned char *data, int inp_width, int inp_height) {
	// To read/write from/to file
	FILE *file;

	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(filename,"wb"))) {
		cout << "Cannot open file: " << filename << endl;
		exit(1);
	}
	fwrite(data, sizeof(unsigned char), (inp_width) * (inp_height), file);
	fclose(file);
}
