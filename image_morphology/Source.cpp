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

void imageDilation(unsigned char *image1, unsigned char *image2, int width, int height, int maskHeight, int maskWidth, string name)
{
	const char *s = name.c_str();
	unsigned char *mask = new unsigned char[maskHeight * maskWidth];
	for (int i = 0; i < maskHeight; i++)
	{
		for (int j = 0; j < maskWidth; j++)
		{
			mask[i * maskWidth + j] = 255;
		}
	}
	int xBoundry = (maskHeight - 1) / 2, yBoundry = (maskWidth - 1) / 2;
	for (int i = xBoundry; i < height - xBoundry; i++)
	{
		for (int j = yBoundry; j < width - yBoundry; j++)
		{
			bool flag = 0;
			for (int mh = -xBoundry; mh < xBoundry + 1; mh++)
			{
				for (int mw = -yBoundry; mw < yBoundry + 1; mw++)
				{
					if (image1[(i + mh) * width + (j + mw)] == mask[(mh + xBoundry) * maskWidth + (mw + yBoundry)])
					{
						flag = 1;
					}
					else
					{
						image2[(i + mh) * width + (j + mw)] = 0;
					}
				}
			}
			if (flag == 1)
			{
				for (int mh = -xBoundry; mh < xBoundry + 1; mh++)
				{
					for (int mw = -yBoundry; mw < yBoundry + 1; mw++)
					{
						image2[(i + mh) * width + (j + mw)] = 255;
					}
				}
			}
		}
	}
	Mat newImage(height, width, CV_8U, image2);
	imwrite(s, newImage);
}




void imageErosion(unsigned char *image1, unsigned char *image2, int width, int height, int maskHeight, int maskWidth, string name)
{
	const char *s = name.c_str();
	int xBoundry = (maskHeight - 1) / 2, yBoundry = (maskWidth - 1) / 2;
	for (int i = xBoundry; i < height - xBoundry; i++)
	{
		for (int j = yBoundry; j < width - yBoundry; j++)
		{
			bool flag = 0;
			for (int mh = -xBoundry; mh < xBoundry+1; mh++)
			{
				for (int mw = -yBoundry; mw < yBoundry+1; mw++)
				{
					if (image1[(i + mh) * width + (j + mw)] == 0)
					{
						flag = 1;
						break;
					}
				}
			}
			if (flag == 1)
			{
				image2[i * width + j] = 0;
			}
			else
			{
				image2[i * width + j] = 255;
			}
		}
	}
	Mat imageRro(height, width, CV_8U, image2);
	imwrite(s, imageRro);
}


int main()
{
	int height = 180, width = 360;
	unsigned char *imageLetter = new unsigned char[height * width];
	readImage(imageLetter, "letters_360x180.raw", height, width);
	unsigned char *imageOutput = new unsigned char[height * width];
	unsigned char *imageOutput2 = new unsigned char[height * width];
	imageErosion(imageLetter, imageOutput, width, height, 3, 3, "imageErosion.png");
	imageDilation(imageLetter, imageOutput2, width, height, 5, 5, "imageDilation.png");

	delete[] imageLetter;
	system("pause");
	return 0;
}