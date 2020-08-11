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

double calMSE(unsigned char *image1, unsigned char *image2, int height, int width, string name)
{
	double MSE = 0;
	for (int i = 0; i < height * width; i++)
	{
		MSE = MSE + (image1[i] - image2[i]) * (image1[i] - image2[i]);
	}
	cout << int(image1[0]) << int(image2[0]) << endl;
	cout << int(image2[0]) << endl;
	MSE = MSE / (height * width);
	cout << name <<  " MSE: " << MSE << endl;
	return MSE;
}

void calPSNR(double MSE, string name)
{
	double PSNR = 0;
	PSNR = 10 * log10(255 * 255 / MSE);
	cout << name << " PSNR: " << PSNR << endl;
}

unsigned char *zeroPadding(unsigned char *image1, int height, int width, int boundry, double mask[], int maskNum, string name)
{
	unsigned char *image2 = new unsigned char[(height) * (width)];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0;
			for (int mh = 0; mh < maskNum; mh++)
			{
				for (int mw = 0; mw < maskNum; mw++)
				{
					int xx = i + mh - boundry, yy = j + mw - boundry;
					if (xx < 0 || xx >= height || yy < 0 || yy >= width)
					{
						value = value + 0 * mask[mh * maskNum + mw];
					}
					else
					{
						value = value + image1[xx * width + yy] * mask[mh * maskNum + mw];
					}
				}
			}
			if (value > 255)
			{
				value = 255;
			}
			else if (value < 0)
			{
				value = 0;
			}
			image2[i * width + j] = value;
		}
	}
	double MSE = calMSE(image1, image2, height, width, name);
	calPSNR(MSE, name);

	Mat image(height, width, CV_8U, image2);
	imwrite(name, image);
	return image2;

}


unsigned char *mirroredPadding(unsigned char *image1, int height, int width, int boundry, double mask[], int maskNum, string name)
{
	unsigned char *image2 = new unsigned char[(height) * (width)];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0;
			for (int mh = 0; mh < maskNum; mh++)
			{
				for (int mw = 0; mw < maskNum; mw++)
				{
					int xx = i + mh - boundry, yy = j + mw - boundry;
					if (xx < 0)
					{
						xx = abs(xx);
					}
					else if (xx >= height)
					{
						xx = height - 1 - abs(height - xx);
					}
					if (yy < 0)
					{
						yy = abs(yy);
					}
					else if (yy >= width)
					{
						yy = width - 1 - abs(width - yy);
					}
					value = value + image1[xx * width + yy] * mask[mh * maskNum + mw];
				}
			}
			if (value > 255)
			{
				value = 255;
			}
			else if (value < 0)
			{
				value = 0;
			}
			image2[i * width + j] = value;
		}
	}
	double MSE = calMSE(image1, image2, height, width, name);
	calPSNR(MSE, name);

	Mat image(height, width, CV_8U, image2);
	imwrite(name, image);
	return image2;
}

unsigned char *replicatedPadding(unsigned char *image1, int height, int width, int boundry, double mask[], int maskNum, string name)
{
	unsigned char *image2 = new unsigned char[(height) * (width)];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0;
			for (int mh = 0; mh < maskNum; mh++)
			{
				for (int mw = 0; mw < maskNum; mw++)
				{
					int xx = i + mh - boundry, yy = j + mw - boundry;
					if (xx < 0)
					{
						xx = 0;
					}
					else if (xx >= height)
					{
						xx = height - 1;
					}
					if (yy < 0)
					{
						yy = 0;
					}
					else if (yy >= width)
					{
						yy = width - 1;
					}
					value = value + image1[xx * width + yy] * mask[mh * maskNum + mw];
				}
			}
			if (value > 255)
			{
				value = 255;
			}
			else if (value < 0)
			{
				value = 0;
			}
			image2[i * width + j] = value;
		}
	}
	double MSE = calMSE(image1, image2, height, width, name);
	calPSNR(MSE, name);

	Mat image(height, width, CV_8U, image2);
	imwrite(name, image);
	return image2;
}

void gaussianFilter(double mask[25], int height, int width, double theta)
{
	double sum = 0, boundry = 2;
	int xx, yy;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0, filter = 0, value2 = 0;
			xx = i - boundry, yy = j - boundry;
			if (xx < 0) { xx = abs(xx); }
			if (yy < 0) { yy = abs(yy); }
			value = (xx * xx + yy * yy), value2 = 1.0000 / (2 * 3.1415926 * theta * theta);
			filter = (exp((-value) / (2 * theta * theta))) * value2;
			
			mask[(i)* width + (j)] = filter;
			sum = sum + filter;
		}
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mask[(i)* width + (j)] = mask[(i)* width + (j)] / sum;
		}
	}

}

void dogFilter(double mask[25], int height, int width, double theta1, double theta2)
{
	double sum = 0, boundry = 2;
	int xx, yy;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0, filter = 0, value2 = 0, value3 = 0;
			xx = i - boundry, yy = j - boundry;
			if (xx < 0) { xx = abs(xx); }
			if (yy < 0) { yy = abs(yy); }
			value = (exp(-(xx * xx + yy * yy) / (2 * theta1 * theta1))) / theta1, value2 = (exp(-(xx * xx + yy * yy) / (2 * theta2 * theta2))) / theta2;
			value3 = (value - value2) / sqrt(2.000 * 3.14159);
			if (value3 < 0)
			{
				value3 = 0;
			}
			else if (value3 > 255)
			{
				value3 = 255;
			}

			mask[(i) * width + (j)] = value3;
	
		}
	}
}

void logFilter(double mask[25], int height, int width, double theta1)
{
	double sum = 0, boundry = 2;
	int xx, yy;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			double value = 0.00000, filter = 0.00000, value2 = 0.000000, value3 = 0.00000;
			xx = i - boundry, yy = j - boundry;
			if (xx < 0) { xx = abs(xx); }
			if (yy < 0) { yy = abs(yy); }
			value3 = -1 * (1 / (3.14159 * theta1 * theta1* theta1*theta1)) * (1 - ((xx * xx + yy * yy) / (2 * theta1*theta1))) * exp(-1 * (xx * xx + yy * yy) / (2 * theta1*theta1));
			mask[(i) * width + (j)] = value3;
			cout << value3 << " ";
			sum = sum + value3;
		}
		cout << endl;
	}
}




int main()
{
	int height = 256, width = 256;
	unsigned char *imageBaboon = new unsigned char[height * width];
	readImage(imageBaboon, "baboon_256.raw", width, height);
	double mask1[25] = { 0 }, mask2[25] = { 0 }, mask3[25] = { 0 };
	gaussianFilter(mask1, 5, 5, 0.8);
	gaussianFilter(mask2, 5, 5, 1.3);
	gaussianFilter(mask3, 5, 5, 2.0);
	mirroredPadding(imageBaboon, height, width, 2, mask1, 5, "gaussian_1.png");
	mirroredPadding(imageBaboon, height, width, 2, mask2, 5, "gaussian_2.png");
	mirroredPadding(imageBaboon, height, width, 2, mask3, 5, "gaussian_3.png");
	double mask4[25] = { 0 }, mask5[25] = { 0 };
	dogFilter(mask4, 5, 5, 0.5, 1.5);
	logFilter(mask5, 5, 5, 0.5);
	mirroredPadding(imageBaboon, height, width, 2, mask4, 5, "doggaussian.png");
	mirroredPadding(imageBaboon, height, width, 2, mask5, 5, "loggaussian.png");
	

	delete[] imageBaboon;
	
	system("pause");
	return 0;
}