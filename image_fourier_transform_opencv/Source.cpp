#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <math.h>
#include <complex>
#include <omp.h>
#include <time.h>
using namespace cv;
using namespace std;

double START, END;


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

tuple<Mat, Mat> dft(Mat &image, int height, int width)
{
	Mat padded;                            //expand input image to optimal size
	int m = getOptimalDFTSize(image.rows);
	int n = getOptimalDFTSize(image.cols); // on the border add zero values
	copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
	dft(complexI, complexI);            // this way the result may fit in the source matrix
	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	Mat magn, angl;
	cartToPolar(planes[0], planes[1], magn, angl, true);
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];
	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);
	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a										// viewable image form (float between values 0 and 1).

	return make_tuple(magI, complexI);
}

Mat iddft(Mat image)
{
	Mat ifft;
	idft(image, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);
	return ifft;
}





void showImage(Mat imageName, string name)
{
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, imageName);
}


int main()
{
	int height = 256.0, width = 256.0;
	unsigned char *imageBlack = new unsigned char[height * width];
	unsigned char *imageSine = new unsigned char[height * width];
	readImage(imageBlack, "blackwhite256_rotate.raw", height, width);
	readImage(imageSine, "sine16_256.raw", height, width);
	Mat black(height, width, CV_8U, imageBlack);
	Mat sine(height, width, CV_8U, imageSine);
	Mat blackspect = get<0>(dft(black, height, width));
	normalize(blackspect, blackspect, 0, 255, CV_MINMAX);
	imwrite("imageBlackPhase.png", blackspect);
	Mat blackreal = get<1>(dft(black, height, width));
	Mat blackorigin = iddft(blackreal);
	normalize(blackorigin, blackorigin, 0, 255, CV_MINMAX);
	imwrite("blackOrigin.png", blackorigin);
	Mat sinespect = get<0>(dft(sine, height, width));
	normalize(sinespect, sinespect, 0, 255, CV_MINMAX);
	imwrite("imageSinePhase.png", sinespect);
	Mat sinereal = get<1>(dft(sine, height, width));
	Mat sineorigin = iddft(sinereal);
	normalize(sineorigin, sineorigin, 0, 255, CV_MINMAX);
	imwrite("sineOrigin.png", sineorigin);


	delete[] imageBlack, imageSine;

	system("pause");
	return 0;
}

