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
		for (int j = 0; j < width ; j++)
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

int main()
{
	int height1 = 720, width1 = 480, padding = 2;
	unsigned char *imageLighthouse = new unsigned char[height1 * width1];
	readImage(imageLighthouse, "lighthouse_480x720.raw", width1, height1);
	double maskLaplicain1[] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 };
	double maskLaplicain2[] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
	double maskSobel1[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	double maskSobel2[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

	zeroPadding(imageLighthouse, height1, width1, 2, maskLaplicain1, 3, "ZeroLaplicain1.png");
	zeroPadding(imageLighthouse, height1, width1, 2, maskLaplicain2, 3, "ZeroLaplicain2.png");
	replicatedPadding(imageLighthouse, height1, width1, 2, maskLaplicain1, 3, "replicatedLaplicain1.png");
	replicatedPadding(imageLighthouse, height1, width1, 2, maskLaplicain2, 3, "replicatedLaplicain2.png");
	replicatedPadding(imageLighthouse, height1, width1, 2, maskSobel1, 3, "replicatedSobel1.png");
	replicatedPadding(imageLighthouse, height1, width1, 2, maskSobel2, 3, "replicatedSobel2.png");
	delete[] imageLighthouse;

	int height2 = 256, width2 = 256;
	unsigned char *blackWhite = new unsigned char[height2 * width2];
	unsigned char *chessboard = new unsigned char[height2 * width2];
	readImage(blackWhite, "block_white_256.raw", width2, height2);
	readImage(chessboard, "chessboard_256.raw", width2, height2);
	double maskMean3[] = { 1.0000 / 9, 1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 ,1.0000 / 9 };
	double maskMean5[] = { 0.04, 0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04 };
	replicatedPadding(blackWhite, height2, width2, 2, maskMean3, 3, "replicatedMean1.png");
	replicatedPadding(blackWhite, height2, width2, 4, maskMean5, 5, "replicatedMean2.png");
	//replicatedPadding(chessboard, height2, width2, 2, maskMean3, 3, "replicatedMean1.png");
	//replicatedPadding(chessboard, height2, width2, 4, maskMean5, 5, "replicatedMean2.png");
	delete[] chessboard, blackWhite;

	system("pause");
	return 0;
}