#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <math.h>
#include <complex>
#include <omp.h>
#include <time.h>
using namespace cv;
using namespace std;



void readImage(unsigned char *image, string path, int height, int width)
{
	const char *s = path.c_str();
	FILE *f = fopen(s, "rb");
	fread(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}

void saveImage(unsigned char *image, string name, int height, int width)
{
	const char *s = name.c_str();
	FILE *f = fopen(s, "wb");
	fwrite(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}



Mat image_make_border(Mat &src)
{
	int w = getOptimalDFTSize(src.cols);
	int h = getOptimalDFTSize(src.rows);
	Mat padded;
	copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32F);
	return padded;
}



tuple<float*, float*> handDFT(unsigned char *image, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if ((i + j) % 2 == 1)
			{
				image[i * width + j] = image[i * width + j] * -1;
			}
		}
	}
	double PI_2 = 2.0 * CV_PI;
	double angleTerm = 0.0;
	double invP = 1.0 / height; // 注意
	double invQ = 1.0 / width; // 注意
	float *Real = new float[height * width];
	float *Imag = new float[height * width];
	float *Spectrum = new float[height * width];
	float *Phase = new float[height * width];
	for (int v = 0; v < height; ++v)
	{
		#pragma omp for
		double v_invQ = (double)(v)*invQ; // 加速計算
		for (int u = 0; u < width; ++u)
		{
			#pragma omp for
			double u_invP = (double)(u)*invP;
			double rd = 0.0; // 實部加總
			double id = 0.0; // 虛部加總
			for (int y = 0; y < height; ++y)
			{
				#pragma omp for
				double vy_invQ = (double)(y)*v_invQ;
				for (int x = 0; x < width; ++x)
				{
					#pragma omp for
					angleTerm = PI_2 * ((double)(x)*u_invP + vy_invQ);
					double target = image[y * width + x]; // f(x,y)

					rd += (double)(target*cos(angleTerm));

					id -= (double)(target*sin(angleTerm));
				}
			}
			Real[v * width + u] = rd;
			Imag[v * width + u] = id;

			// 參見公式
			Spectrum[v * width + u] = log10((double)(sqrt(rd*rd + id * id)));
			Phase[v * width + u] = (double)(atan2(id, rd));
		}
	}
	double max1 = 0, max2 = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (max1 < Real[i * width + j])
			{
				max1 = Real[i * width + j];

			}
			if (max2 < Imag[i * width + j])
			{
				max2 = Imag[i * width + j];
			}
		}
	}
	max1 = 255 / max1, max2 = 255 / max2;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Real[i * width + j] = round(Real[i * width + j] * max1);
			Imag[i * width + j] = round(Imag[i * width + j] * max2);
		}
	}
	Mat SpectrumImage(height, width, CV_32F, Spectrum);
	Mat PhaseImage(height, width, CV_32F, Phase);
	normalize(SpectrumImage, SpectrumImage, 0, 1, CV_MINMAX);
	normalize(PhaseImage, PhaseImage, 0, 1, CV_MINMAX);
	return make_tuple(Real, Imag);
}

Mat handIDFT(float *Real, float *Imag, int height, int width)
{
	double PI_2 = 2.0 * CV_PI;
	double angleTerm = 0.0;
	double invP = 1.0 / height; // 注意
	double invQ = 1.0 / width; // 注意
	double invPQ = invP * invQ;
	double cosineA, sineA;
	float *Recover = new float[height * width];
	for (int y = 0; y < height; ++y)
	{
		double y_invQ = (double)(y)*invQ;
		for (int x = 0; x < width; ++x)
		{
			double x_invP = (double)(x)*invP;
			double inv_rd = 0.0; // 加總實部
			double inv_id = 0.0; // 加總虛部

			for (int v = 0; v < height; ++v)
			{
				double vy_invQ = (double)(v)*y_invQ;
				for (int u = 0; u < width; ++u)
				{
					angleTerm = PI_2 * ((double)(u)*x_invP + vy_invQ);
					cosineA = cos(angleTerm);
					sineA = sin(angleTerm);
					double rd = Real[v * width + u]; // Re(F(u,v))
					double id = Imag[v * width + u]; // Im(F(u,v))

					// 實部係數為( real * cos x - imag * sin x )
					// 虛數係數為( real * sin x + imag * cos x )
					inv_rd += (rd*cosineA - id * sineA); // 實*實 - 虛*虛(變號)
					inv_id += (rd*sineA + id * cosineA); // 實*虛 + 虛*實
				}
			}
			inv_rd *= invPQ; // 對應公式上的 1/MN
			inv_id *= invPQ;
			//double odd_mul = ( (x+y)&1 ) ? -1 : 1; // 不知為何不用乘-1, 待釐清
			// 實部平方 + 虛部平方, 再開根號
			Recover[y * width + x] = (double)(sqrt(inv_rd*inv_rd + inv_id * inv_id));
		}
	}

	Mat Recovertest(height, width, CV_32F, Recover);
	Mat rec_out;
	Recovertest.convertTo(rec_out, CV_8U);
	return Recovertest;
}


void showImage(Mat imageName, string name)
{
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, imageName);
}


int main()
{
	int height = 256, width = 256;
	unsigned char *black = new unsigned char[height * width];
	readImage(black, "blackwhite256_rotate.raw", height, width);
	float *test = get<0>(handDFT(black, height, width));
	float *test2 = get<1>(handDFT(black, height, width));
	Mat testReocver = handIDFT(test, test2, height, width);
	imwrite("Recover3.png", testReocver);
	system("pause");
	return 0;
}

