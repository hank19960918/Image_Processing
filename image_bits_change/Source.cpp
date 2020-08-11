#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
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

void changeBits(unsigned char *image, int bits, int height, int width)
{
	int k = 255;
	int pows = 8 - bits;
	for (int i = 0; i < height * width; i++)
	{
		image[i] = image[i] >> bits;
		image[i] = image[i] * 255 / (pow(2, pows) - 1);
	}
}

double calMSE(unsigned char *image1, unsigned char *image2, int height, int width)
{
	double MSE = 0;
	for (int i = 0; i < height * width; i++)
	{
		MSE = MSE + (image1[i] - image2[i]) * (image1[i] - image2[i]);
	}
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

void chooseMenu()
{
	cout << "enter 1 to use 8 bits to present 8 bits" << endl;
	cout << "enter 2 to use 7 bits to present 8 bots" << endl;
	cout << "enter 3 to use 6 bits to present 8 bits" << endl;
	cout << "enter 4 to use 5 bits to present 8 bits" << endl;
	cout << "enter 5 to use 4 bits to present 8 bits" << endl;
	cout << "enter 6 to use 3 bits to present 8 bits" << endl;
	cout << "enter 7 to use 2 bits to present 8 bits" << endl;
	cout << "enter 8 to use 1 bits to present 8 bits" << endl;
}

int main()
{
	int height = 256, width = 256;
	string lenaPath = "lena_256.raw";
	
	while (true)
	{
		chooseMenu();
		int bitsChoose = 0;
		double MSE = 0;
		cout << "enter change bits: " << endl; 
		cin >> bitsChoose;
		unsigned char *imageLena = new unsigned char[height * width];
		unsigned char *lenaOriginal = new unsigned char[height * width];
		readImage(lenaOriginal, lenaPath, height, width);
		readImage(imageLena, lenaPath, height, width);
		if (bitsChoose == 1)
		{
			changeBits(imageLena, 0, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena8bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 2)
		{
			changeBits(imageLena, 1, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena7bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 3)
		{
			changeBits(imageLena, 2, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena6bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 4)
		{
			changeBits(imageLena, 3, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena5bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 5)
		{
			changeBits(imageLena, 4, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena4bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 6)
		{
			changeBits(imageLena, 5, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena3bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 7)
		{
			changeBits(imageLena, 6, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena2bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		else if (bitsChoose == 8)
		{
			changeBits(imageLena, 7, height, width);
			Mat lena(256, 256, CV_8U, imageLena);
			imwrite("lena1bits.png", lena);
			MSE = calMSE(imageLena, lenaOriginal, height, width);
			calPSNR(MSE);
		}
		delete[] lenaOriginal;
	}
	system("pause");

	return 0;
}