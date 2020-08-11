#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <time.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;
void menu()
{
	cout << "enter 1 to show lena256.raw" << endl;
	cout << "enter 2 to rotate lena256.raw" << endl;
	cout << "enter 3 to split lena256.raw" << endl;
	cout << "enter 4 to bright lena256.raw" << endl;
	cout << "enter 5 to draw something on lena512.raw" << endl;
}
unsigned char *readImage(int height, int width, string fileName)
{
	const char *c = fileName.c_str();
	unsigned char *readImage = new unsigned char[height * width];
	FILE *f = fopen(c, "rb");
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			fread(&readImage[i * width + j], sizeof(unsigned char), 1, f);
		}
	}
	fclose(f);
	return readImage;
}

unsigned char *rotatedImage(int height, int width, unsigned char *imageOriginal)
{
	unsigned char *imageRotated = new unsigned char[height * width];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageRotated[(width - 1 - j)* width + i] = imageOriginal[i * width + j];
		}
	}
	return imageRotated;
}

unsigned char *flipImage(int height, int width, unsigned char *imageOriginal)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0, k = width - 1; j < k; j++, k--)
		{
			swap(imageOriginal[j * width + i], imageOriginal[k * width + i]);
		}
	}
	return imageOriginal;
}

unsigned char *brightImage(int height, int width, unsigned char *imageOriginal)
{
	unsigned char *imageBrightness = new unsigned char[height * width];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageBrightness[i * width + j] = imageOriginal[i * width + j];
			if (imageBrightness[i * width + j] + 60 > 255)
			{
				imageBrightness[i * width + j] = 255;
			}
			else
			{
				imageBrightness[i * width + j] = imageBrightness[i * width + j] + 60;
			}
		}
	}
	return imageBrightness;
}

unsigned char *imageSplit(int height, int width, unsigned char *imageOriginal)
{
	unsigned char *imageSplit = new unsigned char[height * width];
	array<int, 8>  c = { 1, 2, 3, 4, 5, 6, 7, 8 };
	srand(time(NULL));
	random_shuffle(c.begin(), c.end());
	int newRow = 0;
	for (int row = 0; row < 8; row++)
	{
		// set the max row = 256
		int max = c[row] * 32;
		// set the min row = 224
		int min = (c[row] - 1) * 32;
		// change from the max row to min row to the new position
		for (int i = min; i < max; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				imageSplit[newRow * width + j] = imageOriginal[i * width + j];
			}
			newRow++;
		}
	}
	return imageSplit;
}

void imageWindow(string fileName, Mat imageName)
{
	const char *c = fileName.c_str();
	namedWindow(c, WINDOW_AUTOSIZE);
	imshow("lena256", imageName);
	waitKey(0);
}

int main() 
{
	while (true)
	{
		int height = 256, width = 256;
		unsigned char *lenaOriginal = new unsigned char[height * width];
		// read the lena_256.raw image data
		lenaOriginal = readImage(height, width, "lena_256.raw");
		Mat lena256(height, width, CV_8U, lenaOriginal);

		// rotate the image 
		unsigned char *lenaRotate = new unsigned char[height * width];
		lenaRotate = rotatedImage(height, width, lenaOriginal);
		// flip the image to get the result we want
		lenaRotate = flipImage(height, width, lenaRotate);
		Mat lenaRotat(height, width, CV_8U, lenaRotate);
		imwrite("lenaRotated90.png", lenaRotat);

		unsigned char *lenaBrightness = new unsigned char[height * width];
		lenaBrightness = brightImage(height, width, lenaOriginal);
		Mat lenaBright(height, width, CV_8U, lenaBrightness);
		imwrite("lenaBrightness.png", lenaBright);

		unsigned char *lenaSlice = new unsigned char[height * width];
		lenaSlice = imageSplit(height, width, lenaOriginal);
		Mat lenaRotat52(height, width, CV_8U, lenaSlice);
		imwrite("lenaSlice.png", lenaRotat52);

		int height512 = 512, width512 = 512;
		unsigned char *lena512 = new unsigned char[height512 * width512];
		lena512 = readImage(height512, width512, "lena_512.raw");
		Mat lena5122(height512, width512, CV_8U, lena512);
		menu();
		int choose;
		cin >> choose;
		if (choose == 1)
		{
			imageWindow("lena256", lena256);
		}
		else if (choose == 2)
		{
			imageWindow("lenaRotate90", lenaRotat);
		}
		else if (choose == 3)
		{
			imageWindow("lenaSlice", lenaRotat52);
		}
		else if (choose == 4)
		{
			imageWindow("lenaBright", lenaBright);
		}
		else if (choose == 5)
		{
			imageWindow("lenaRotate512", lena5122);
		}
	}
	system("pause");
	return 0;
}



