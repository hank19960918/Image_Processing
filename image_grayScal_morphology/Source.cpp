#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void readImage(unsigned char *image, string path, int height, int width)
{
	const char *s = path.c_str();
	FILE *f = fopen(s, "rb");
	fread(image, sizeof(unsigned char), height * width, f);
	fclose(f);
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
	int num = size - 1;
	sort(count, count + size);
	return count[num];
}

void grayDilation(unsigned char *image2, int height, int width, string name, int size, int padding, int maskSize)
{
	int boundry = padding / 2;
	int *countTest = new int[size];
	unsigned char *resultZero = new unsigned char[height * width];
	for (int i = boundry; i < height + boundry; i++)
	{
		for (int j = boundry; j < width + boundry; j++)
		{
			int max = 0;
			for (int mh = 0; mh < maskSize; mh++)
			{
				for (int mw = 0; mw < maskSize; mw++)
				{
					//countTest[mh * maskSize + mw] = image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
					//value = value +image2[(i - boundry + mh) * (width + 2) + (j - boundry + mw)];
					if (max < image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)])
					{
						max = image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
					}
				}
			}
			int pixel = sortMask(countTest, size);
			resultZero[(i - boundry) * (width)+(j - boundry)] = max;
		}
	}
	Mat image(height, width, CV_8U, resultZero);
	imwrite(name, image);
	delete[] resultZero, countTest;
}

int sortMask2(int *count, int size)
{
	sort(count, count + size);
	return count[0];
}
void grayErosion(unsigned char *image2, int height, int width, string name, int size, int padding, int maskSize)
{
	int boundry = padding / 2;
	int *countTest = new int[size];
	unsigned char *resultZero = new unsigned char[height * width];
	for (int i = boundry; i < height + boundry; i++)
	{
		for (int j = boundry; j < width + boundry; j++)
		{
			int min = 100000;
			for (int mh = 0; mh < maskSize; mh++)
			{
				for (int mw = 0; mw < maskSize; mw++)
				{
					//countTest[mh * maskSize + mw] = image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
					//value = value +image2[(i - boundry + mh) * (width + 2) + (j - boundry + mw)];
					if (min > image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)])
					{
						min = image2[(i - boundry + mh) * (width + padding) + (j - boundry + mw)];
					}
				}
			}
			int pixel = sortMask2(countTest, size);
			if (pixel - 1 == 0)
			{
				pixel = 0;
			}
			resultZero[(i - boundry) * (width)+(j - boundry)] = min ;
		}
	}
	Mat image(height, width, CV_8U, resultZero);
	imwrite(name, image);
	delete[] resultZero, countTest;
}

int main()
{
	
	int height = 609, width = 480;
	unsigned char *imageground = new unsigned char[height * width];
	readImage(imageground, "ground_480x609.raw", height, width);
	unsigned char *imagegroundpadding = new unsigned char[(height + 4) * (width + 4)];
	imagegroundpadding = replicatedPadding(imageground, height, width, 4);
	grayErosion(imagegroundpadding, height, width, "groundErosion.png", 25, 4, 5);
	grayDilation(imagegroundpadding, height, width, "groundDilation.png", 25, 4, 5);


	
	system("pause");
	return 0;
}