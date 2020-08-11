#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;
using namespace cv;

void readImage(unsigned char *image, string path, int height, int width)
{
	const char *s = path.c_str();
	FILE *f = fopen(s, "rb");
	fread(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}

void showImage(Mat imageName, string name, unsigned char *image)
{
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, imageName);
	delete[] image;
	waitKey(0);
}

void saveImage(unsigned char *image, string name, int height, int width)
{
	const char *s = name.c_str();
	FILE *f = fopen(s, "wb");
	fwrite(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}

void calNegative(unsigned char *image1, unsigned char *image2, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image2[i * width + j] = 255 - image1[i * width + j];
		}
	}
}

void calSDR(unsigned char *image1, unsigned char *image2, int height, int width)
{
	double pixel = 0;
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			pixel = image1[i * width + j] * 1.0000000000 / 255;
			if (pixel < 0.018)
			{
				pixel = pixel * 4.5;
			}
			else if (pixel >= 0.018)
			{
				pixel = 1.099 * pow(pixel, 0.45) - 0.099;
			}
			image2[i * width + j] = pixel * 255;
		}
	}
}

void calHLG(unsigned char *image1, unsigned char *image2, int height, int width)
{
	double pixel;
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			pixel = image1[i * width + j] * 1.0000000000 / 255;
			if (1 >= pixel > 0.0833333333333333333333333)
			{
				pixel = 0.17883277 * log(12 * pixel - 0.28466892) + 0.55991073;
			}
			else if (0.0833333333333333333333333 >= pixel >= 0)
			{
				pixel = sqrt(3) * pow(pixel, 0.5);
			}
			image2[i * width + j] = pixel * 255;
		}
	}
}

void bitOperation(unsigned char *image1,int height, int width, int operationNumber, int time, string name)
{
	const char *s = name.c_str();
	unsigned char *image2 = new unsigned char[height * width];
	int value = pow(2, time) - 1;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image2[i * width + j] = (image1[i * width + j] & operationNumber) * value;
		}
	}
	Mat imageBitoperation(height, width, CV_8U, image2);
	imwrite(s, imageBitoperation);
	delete[] image2;
}


int main() 
{
	int height = 256, width = 256;
	unsigned char *imageBaboon256 = new unsigned char[height * width];
	readImage(imageBaboon256, "baboonQ_256.raw", height, width);

	bitOperation(imageBaboon256, height, width, 128, 1, "Baboon8bits.png");
	bitOperation(imageBaboon256, height, width, 64, 2, "Baboon7bits.png");
	bitOperation(imageBaboon256, height, width, 32, 3, "Baboon6bits.png");
	bitOperation(imageBaboon256, height, width, 16, 4, "Baboon5bits.png");
	bitOperation(imageBaboon256, height, width, 8, 5, "Baboon4bits.png");
	bitOperation(imageBaboon256, height, width, 4, 6, "Baboon3bits.png");
	bitOperation(imageBaboon256, height, width, 2, 7, "Baboon2bits.png");
	bitOperation(imageBaboon256, height, width, 1, 8, "Baboon1bits.png");



	delete[] imageBaboon256;
	
	
	waitKey(0);
	system("pause");
	return 0;
}

