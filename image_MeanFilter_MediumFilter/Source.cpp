#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <complex>
#include <random>
#define imgi   complex<float> (0.0,1.0) 
#define Real   complex<float> (10.0,0.0)
#define Real1   complex<float> (0.0,0.0)
#define Real2   complex<float> (1.0,0.0)
#define pi 3.14159265359  
using namespace cv;
using namespace std;

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

unsigned char *replicatedPadding(unsigned char *image1, int height, int width, int padding)
{
	int boundry = padding / 2, xx = 0, yy = 0;
	unsigned char *image2 = new unsigned char[(height + padding) * (width + padding)];
	for (int i = 0; i < height + padding; i++)
	{
		for (int j = 0; j < width + padding; j++)
		{
			//image2[(i + boundry) * (width + padding) + (j + boundry)] = image1[i * width + j];
			xx = i - boundry, yy = j - boundry;
			if (xx > height - 1)
			{
				xx = height - 1;
			}
			if (i - boundry < 0)
			{
				xx = 0;
			}
			if (j - boundry < 0)
			{
				yy = 0;
			}
			if (yy > width - 1)
			{
				yy = width - 1;
			}
			image2[i * (width + padding) + j] = image1[xx * width + yy];
		}
	}
	Mat image(height + padding, width + padding, CV_8U, image2);
	return image2;
}

int sortMask(int *count, int size)
{
	int num = size / 2;
	sort(count, count + size);
	return count[num];
}

void mediumFilter(unsigned char *image2, int height, int width, string name, int size, int padding, int maskSize)
{
	int boundry = padding / 2;
	int *countTest = new int [size];
	unsigned char *resultZero = new unsigned char[height * width];
	for (int i = boundry; i < height + boundry; i++)
	{
		for (int j = boundry; j < width + boundry; j++)
		{
			for (int mh = 0; mh < maskSize; mh++)
			{
				for (int mw = 0; mw < maskSize; mw++)
				{
					countTest[mh * maskSize + mw] = image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
					//value = value +image2[(i - boundry + mh) * (width + 2) + (j - boundry + mw)];
				}
			}
			int pixel = sortMask(countTest, size);
			resultZero[(i - boundry) * (width)+(j - boundry)] = pixel;
		}
	}
	Mat image(height, width , CV_8U, resultZero);
	imwrite(name, image);
	delete[] resultZero, countTest;
}

void meanFilter(unsigned char *image2, int height, int width, string name, int size, int padding, int maskSize)
{
	int boundry = padding / 2;
	unsigned char *resultZero = new unsigned char[height * width];
	for (int i = boundry; i < height + boundry; i++)
	{
		for (int j = boundry; j < width + boundry; j++)
		{
			double value = 0;
			for (int mh = 0; mh < maskSize; mh++)
			{
				for (int mw = 0; mw < maskSize; mw++)
				{
					value = value + image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
				}
			}
			resultZero[(i - boundry) * (width)+(j - boundry)] = value / double(size);
		}
	}
	Mat image(height, width, CV_8U, resultZero);
	imwrite(name, image);
	delete[] resultZero;
}

Mat addNoise(Mat src)
{
	Mat noise = Mat(src.size(), CV_32F);
	randn(noise, 0, 20);
	src.convertTo(src, CV_32F);
	src = src + noise;
	src.convertTo(src, CV_8U);
	return src;
}

Mat addNoise2(Mat src)
{
	Mat noise = Mat(src.size(), CV_32F);
	randn(noise, 0, 20);
	return noise;
}


int main()
{
	int height = 512, width = 512;
	unsigned char *imageSaltPepper = new unsigned char[height * width];
	readImage(imageSaltPepper, "lena512_salt_pepper.raw", height, width);
	unsigned char *imageSaltPepperMedium1 = new unsigned char[(height + 2) * (width + 2)];
	imageSaltPepperMedium1 = replicatedPadding(imageSaltPepper, height, width, 2);
	mediumFilter(imageSaltPepperMedium1, height, width, "lenaSaltPepperMedium1.png", 9, 2, 3);
	meanFilter(imageSaltPepperMedium1, height, width, "lenaSaltPepperMean1.png", 9, 2, 3);
	delete[] imageSaltPepper, imageSaltPepperMedium1;


	
	system("pause");
	return 0;
 }

