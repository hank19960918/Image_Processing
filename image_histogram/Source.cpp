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

void calPixel(unsigned char *image1, int height, int width, double count[256])
{
	for (int k = 0; k < 256; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (k == image1[i * width + j])
				{
					count[k]++;
				}
			}
		}
	}

}

void calPro(double count[256], double countPro[256], int height)
{
	double pro = 0;
	for (int i = 0; i < 256; i++)
	{
		if (count[i] != 0)
		{
			pro = pro + count[i] / (height * height) * 255;
			countPro[i] = round(pro);
		}
	}
}

void histEqu(unsigned char *image1, double count[256], int height, int width, string name)
{
	const char *s = name.c_str();
	unsigned char *image2 = new unsigned char[height * width];
	for (int k = 0; k < 256; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (k == image1[i * 256 + j])
				{
					image2[i * 256 + j] = count[k];
				}

			}
		}
	}
	Mat result(height, width, CV_8U, image2);
	imwrite(s, result);
	delete[] image2;
}


int main() 
{
	int height = 256, width = 256;
	unsigned char *imageDogBright = new unsigned char[height * width];
	unsigned char *imageDogDark = new unsigned char[height * width];
	unsigned char *imageLena512 = new unsigned char[(height * 2) * (width * 2)];
	readImage(imageDogBright, "dog_bright256.raw", height, width);
	readImage(imageDogDark, "dog_dark256.raw", height, width);
	readImage(imageLena512, "lena_512.raw", height * 2, width * 2);
	double countBright[256] = { 0 }, countDark[256] = { 0 }, countLena512[256] = { 0 };
	calPixel(imageDogBright, height, width, countBright);
	calPixel(imageDogDark, height, width, countDark);
	calPixel(imageLena512, 512, 512, countLena512);
	double countBrightPro[256] = { 0 }, countDarkPro[256] = { 0 }, count512Pro[256] = { 0 }, countTry[256] = { 0 };
	calPro(countBright, countBrightPro, 256);
	calPro(countDark, countDarkPro, 256);

	histEqu(imageDogBright, countBrightPro, height, width, "historgmBrightEqu.png");
	histEqu(imageDogDark, countDarkPro, height, width, "historgmDarkEqu.png");

	delete[] imageDogBright, imageDogDark, imageLena512;

	
	
	waitKey(0);
	system("pause");
	return 0;
}

