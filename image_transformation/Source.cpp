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

void calNegative(unsigned char *image1, int height, int width, string name)
{
	const char *s = name.c_str();
	unsigned char *image2 = new unsigned char[height * width];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image2[i * width + j] = 255 - image1[i * width + j];
		}
	}
	Mat imageNegative(height, width, CV_8U, image2);
	imwrite(s, imageNegative);
	delete[] image2;
}

void calSDR(unsigned char *image1, int height, int width, string name)
{
	const char *s = name.c_str();
	unsigned char *image2 = new unsigned char[height * width];
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
	Mat imageSDR(height, width, CV_8U, image2);
	imwrite(s, imageSDR);
	delete[] image2;
}

void calHLG(unsigned char *image1, int height, int width, string name)
{
	const char *s = name.c_str();
	unsigned char *image2 = new unsigned char[height * width];
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
	Mat imageHLG(height, width, CV_8U, image2);
	imwrite(s, imageHLG);
	delete[] image2;
}

int main() 
{
	int height = 512, width = 512;
	unsigned char *imageLena512 = new unsigned char[height * width];
	readImage(imageLena512, "lena_512.raw", height, width);
	calNegative(imageLena512, height, width, "lenaNegative.png");
	calSDR(imageLena512, height, width, "lenaSDR.png");
	calHLG(imageLena512, height, width, "lenaHLG.png");
	delete[] imageLena512;
	system("pause");
	return 0;
}

