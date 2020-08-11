#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <queue>
#include <tuple>
#include<stack>
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

void saveImage(unsigned char *image, string name, int height, int width)
{
	const char *s = name.c_str();
	FILE *f = fopen(s, "wb");
	fwrite(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}



double calMSE(unsigned char *image1, unsigned char *image2, int height, int width)
{
	double MSE = 0;
	for (int i = 0; i < height * width; i++)
	{
		MSE = MSE + (image1[i] - image2[i]) * (image1[i] - image2[i]);
	}
	cout << int(image1[0]) << int(image2[0]) << endl;
	cout << int(image2[0]) << endl;
	MSE = MSE / (height * width);
	cout << "MSE: " << MSE << endl;
	return MSE;
}

void calPSNR(double MSE)
{
	double PSNR = 0;
	PSNR = 10 * log10(255 * 255 / MSE);
	cout << "PSNR: " << PSNR << endl;
}

void zoomImage(unsigned char *image, unsigned char *imageBig, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageBig[i * height + j] = image[(i / 2) * (height / 2) + (j / 2)];
		}
	}
}

void shrinkImage(unsigned char *image, unsigned char *imageSmall, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imageSmall[i * height + j] = image[(i * 2) * 256 + (j * 2)];
		}
	}
}

double findMin(double dist[4])
{
	double min = dist[0];
	for (int i = 0; i < 4; i++)
	{
		if (dist[i] < min)
		{
			min = dist[i];
		}
	}
	return min;
}

void nearestZoom(unsigned char *image, unsigned char *imageBig, int height, int width, double size, int nweWidth)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double xx = i / size, yy = j / size, distMin[4];
			double min = 0;
			int xOriginal = i / size, yOriginal = j / size, xOriginal_1 = xOriginal + 1, yOriginal_1 = yOriginal + 1;
			distMin[0] = (xx - xOriginal) * (xx - xOriginal) + (yy - yOriginal) * (yy - yOriginal);
			distMin[1] = (xx - xOriginal_1) * (xx - xOriginal_1) + (yy - yOriginal) *-(yy - yOriginal);
			distMin[2] = (xx - xOriginal) * (xx - xOriginal) + (yy - yOriginal_1) * (yy - yOriginal_1);
			distMin[3] = (xx - xOriginal_1) * (xx - xOriginal_1) + (yy - yOriginal_1) * (yy - yOriginal_1);
			min = findMin(distMin);
			if (min == distMin[0])
			{
				imageBig[i * width + j] = image[xOriginal * nweWidth + yOriginal];
			}
			else if (min == distMin[1])
			{
				imageBig[i * width + j] = image[xOriginal_1 * nweWidth + yOriginal];
			}
			else if (min == distMin[2])
			{
				imageBig[i * width + j] = image[xOriginal * nweWidth + yOriginal_1];
			}
			else if (min == distMin[3])
			{
				imageBig[i * width + j] = image[xOriginal_1 * nweWidth + yOriginal_1];
			}
		}
	}
}

void bilinearZoom(unsigned char *image, unsigned char *imageBig, int height, int width, double size, int newWidth)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double xx = i / size, yy = j / size;
			int xOriginal = i / size, yOriginal = j / size, xOriginal_1 = xOriginal + 1, yOriginal_1 = yOriginal + 1;
			double R1 = ((xOriginal_1 - xx) / (xOriginal_1 - xOriginal) * image[xOriginal * newWidth + yOriginal]) + ((xx - xOriginal) / (xOriginal_1 - xOriginal) * image[xOriginal_1 * newWidth + yOriginal]);
			double R2 = ((xOriginal_1 - xx) / (xOriginal_1 - xOriginal) * image[xOriginal * newWidth + yOriginal_1]) + ((xx - xOriginal) / (xOriginal_1 - xOriginal) * image[xOriginal_1 * newWidth + yOriginal_1]);
			imageBig[i * width + j] = ((yOriginal_1 - yy) / (yOriginal_1 - yOriginal) * R1) + ((yy - yOriginal) / (yOriginal_1 - yOriginal) * R2);
		}
	}
}


void getW_x(double W_x[4], double x)
{
	int X = (int)x;
	double a = -0.5;
	double stemp_x[4];
	stemp_x[0] = 1 + (x - X); stemp_x[1] = x - X; stemp_x[2] = 1 - (x - X); stemp_x[3] = 2 - (x - X);
	W_x[0] = a * abs(stemp_x[0] * stemp_x[0] * stemp_x[0]) - 5 * a*stemp_x[0] * stemp_x[0] + 8 * a*abs(stemp_x[0]) - 4 * a;
	W_x[1] = (a + 2)*abs(stemp_x[1] * stemp_x[1] * stemp_x[1]) - (a + 3)*stemp_x[1] * stemp_x[1] + 1;
	W_x[2] = (a + 2)*abs(stemp_x[2] * stemp_x[2] * stemp_x[2]) - (a + 3)*stemp_x[2] * stemp_x[2] + 1;
	W_x[3] = a * abs(stemp_x[3] * stemp_x[3] * stemp_x[3]) - 5 * a*stemp_x[3] * stemp_x[3] + 8 * a*abs(stemp_x[3]) - 4 * a;
}

void getW_y(double W_y[4], double y)
{
	int Y = (int)y;
	double a = -0.5;
	double stemp_y[4];
	stemp_y[0] = 1.0 + (y - Y);
	stemp_y[1] = y - Y;
	stemp_y[2] = 1 - (y - Y);
	stemp_y[3] = 2 - (y - Y);
	W_y[0] = a * abs(stemp_y[0] * stemp_y[0] * stemp_y[0]) - 5 * a*stemp_y[0] * stemp_y[0] + 8 * a*abs(stemp_y[0]) - 4 * a;
	W_y[1] = (a + 2)*abs(stemp_y[1] * stemp_y[1] * stemp_y[1]) - (a + 3)*stemp_y[1] * stemp_y[1] + 1;
	W_y[2] = (a + 2)*abs(stemp_y[2] * stemp_y[2] * stemp_y[2]) - (a + 3)*stemp_y[2] * stemp_y[2] + 1;
	W_y[3] = a * abs(stemp_y[3] * stemp_y[3] * stemp_y[3]) - 5 * a*stemp_y[3] * stemp_y[3] + 8 * a*abs(stemp_y[3]) - 4 * a;
}

void bicubicZoom(unsigned char *image1, unsigned char *image2, int height, int width, double size, int newWidth)
{
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			double xx = i / size, yy = j / size;
			double W_x[4], W_y[4];
			getW_x(W_x, xx);
			getW_y(W_y, yy);
			double temp = 0;
			for (int s = 0; s <= 3; s++)
			{
				for (int t = 0; t <= 3; t++)
				{
					temp = temp + image1[(int(xx) + s - 1) * newWidth + int(yy) + t - 1] * W_x[s] * W_y[t];
				}
			}
			image2[i * width + j] = temp;
		}
	}
}


int main()
{
	int height = 256, width = 256;
	string lenaPath = "lena_256.raw";
	unsigned char *imageLena = new unsigned char[height * width];
	readImage(imageLena, lenaPath, height, width);
	// zoom the image 2 times
	int heightBig = height * 2, widthBig = width * 2;
	unsigned char *imageLenaBig = new unsigned char[heightBig * widthBig];
	zoomImage(imageLena, imageLenaBig, heightBig, widthBig);
	Mat lena512(512, 512, CV_8U, imageLenaBig);
	imwrite("lena512.png", lena512);
	delete[] imageLenaBig;

	// shrink the image 2 times
	int heightSmall = height / 2, widthSmall = width / 2;
	unsigned char *imageLenaSmall = new unsigned char[heightSmall * widthSmall];
	shrinkImage(imageLena, imageLenaSmall, heightSmall, widthSmall);
	Mat lena128(128, 128, CV_8U, imageLenaSmall);
	imwrite("lena128.png", lena128);
	delete[] imageLenaSmall;

	// zoom the image 1.75 times by Nearest
	int heightBig2 = height * 1.75, widthBig2 = width * 1.75;
	unsigned char *imageLenaNearest = new unsigned char[heightBig2 * widthBig2];
	nearestZoom(imageLena, imageLenaNearest, heightBig2, widthBig2, 1.75, 256);
	Mat lenaNearest(448, 448, CV_8U, imageLenaNearest);
	imwrite("lenaNearest.png", lenaNearest);
	delete[] imageLenaNearest;

	// zoom the image 1.75 times by bilinear
	unsigned char *imageLenaBilinear = new unsigned char[heightBig2 * widthBig2];
	bilinearZoom(imageLena, imageLenaBilinear, heightBig2, widthBig2, 1.75, 256);
	Mat lenaBilinear(448, 448, CV_8U, imageLenaBilinear);
	imwrite("lenaBilinear.png", lenaBilinear);
	delete[] imageLenaBilinear;

	// zoom the image 1.75 times by bicubic
	unsigned char *imageLenaBicubic = new unsigned char[heightBig * widthBig];
	bicubicZoom(imageLena, imageLenaBicubic, heightBig2, widthBig2, 1.75, 256);
	Mat lenaBicubic(448, 448, CV_8U, imageLenaBicubic);
	imwrite("lenaBicubic.png", lenaBicubic);
	delete[] imageLenaBicubic, imageLena;


	system("pause");

	return 0;
}