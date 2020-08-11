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



Mat image_make_border(Mat &src)
{
	int w = getOptimalDFTSize(src.cols);
	int h = getOptimalDFTSize(src.rows);
	Mat padded;
	copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32F);
	return padded;
}

Mat frequency_filter(Mat &src, Mat &blur)
{
	Mat plane[] = { src, Mat::zeros(src.size() , CV_32FC1) }; //Create a channel and store the real and imaginary parts after dft (CV_32F, Must be a single channel number)
	Mat complexIm;
	merge(plane, 2, complexIm);//Merge channel (combines two matrices into a 2-channel Mat class container)
	dft(complexIm, complexIm);//Fourier transform, the result is saved in itself

	//***************Centralization********************
	split(complexIm, plane);//separation channel (array separation)
	// plane[0] = plane[0](Rect(0, 0, plane[0].cols & -2, plane[0].rows & -2)); / / Here why and on -2 specific view opencv documentation
	// //In fact, it is to make the rows and columns into even numbers. The binary of 2 is 11111111.......10 The last bit is 0.
	int cx = plane[0].cols / 2; int cy = plane[0].rows / 2;//The following operations are moving images (zero frequency shift to center)
	Mat part1_r(plane[0], Rect(0, 0, cx, cy)); //Element coordinates are expressed as (cx,cy)
	Mat part2_r(plane[0], Rect(cx, 0, cx, cy));
	Mat part3_r(plane[0], Rect(0, cy, cx, cy));
	Mat part4_r(plane[0], Rect(cx, cy, cx, cy));

	Mat temp;
	part1_r.copyTo(temp); //Upper left and lower right exchange position (real part)
	part4_r.copyTo(part1_r);
	temp.copyTo(part4_r);

	part2_r.copyTo(temp); //Upper right and bottom left exchange position (real part)
	part3_r.copyTo(part2_r);
	temp.copyTo(part3_r);

	Mat part1_i(plane[1], Rect(0, 0, cx, cy)); //Element coordinates (cx, cy)
	Mat part2_i(plane[1], Rect(cx, 0, cx, cy));
	Mat part3_i(plane[1], Rect(0, cy, cx, cy));
	Mat part4_i(plane[1], Rect(cx, cy, cx, cy));

	part1_i.copyTo(temp); //Upper left and lower right exchange position (imaginary part)
	part4_i.copyTo(part1_i);
	temp.copyTo(part4_i);

	part2_i.copyTo(temp); // upper right and lower left exchange position (imaginary part)
	part3_i.copyTo(part2_i);
	temp.copyTo(part3_i);

	//***************** The product of the filter function and the DFT result ******************
	Mat blur_r, blur_i, BLUR;
	multiply(plane[0], blur, blur_r); //filter (the real part is multiplied by the corresponding element of the filter template)
	multiply(plane[1], blur, blur_i); // filter (imaginary part is multiplied by the corresponding element of the filter template)
	Mat plane1[] = { blur_r, blur_i };
	merge(plane1, 2, BLUR);//The real and imaginary parts merge

	//*********************Get the original spectrum map********************** *************
	//magnitude(plane[0], plane[1], plane[0]);//Get the amplitude image, 0 channel is the real channel, 1 is the imaginary part, because 2D Fourier The result of the transformation is a complex number
	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			plane[0].at<float>(i, j) = plane[0].at<float>(i, j) * plane[1].at<float>(i, j);
		}
	}


	plane[0] += Scalar::all(1); //The image after Fourier transform is not well analyzed, and the logarithmic processing is performed. The result is better.
	log(plane[0], plane[0]); // The gray space of the float type is [0,1])
	normalize(plane[0], plane[0], 0, 1, CV_MINMAX); //normalized for easy display
	

	idft(BLUR, BLUR); //idft result is also plural
	split(BLUR, plane);//Separate channel, mainly get channel
	magnitude(plane[0], plane[1], plane[0]); //Amplitude (modulo)
	normalize(plane[0], plane[0], 1, 0, CV_MINMAX); //normalized for easy display
	return plane[0];//return parameters
	
}

Mat ideal_low_kernel(Mat &scr, float sigma)
{
	Mat ideal_low_pass(scr.size(), CV_32FC1);
	float d0 = sigma;
	for (int i = 0; i < scr.rows; i++) {
		for (int j = 0; j < scr.cols; j++) {
			double d = sqrt(pow((i - scr.rows / 2), 2) + pow((j - scr.cols / 2), 2));
			if (d <= d0) {
				ideal_low_pass.at<float>(i, j) = 1;
			}
			else {
				ideal_low_pass.at<float>(i, j) = 0;
			}
		}
	}
	return ideal_low_pass;
}

Mat ideal_low_pass_filter(Mat &src, float sigma)
{

	Mat padded = image_make_border(src);
	Mat ideal_kernel = ideal_low_kernel(padded, sigma);
	Mat result = frequency_filter(padded, ideal_kernel);
	return result;
}

Mat ideal_high_kernel(Mat &scr, float sigma)
{
	Mat ideal_high_pass(scr.size(), CV_32FC1);
	float d0 = sigma;
	for (int i = 0; i < scr.rows; i++) {
		for (int j = 0; j < scr.cols; j++) {
			double d = sqrt(pow((i - scr.rows / 2), 2) + pow((j - scr.cols / 2), 2));
			if (d <= d0) {
				ideal_high_pass.at<float>(i, j) = 0;
			}
			else {
				ideal_high_pass.at<float>(i, j) = 1;
			}
		}
	}
	return ideal_high_pass;
}

Mat ideal_high_pass_filter(Mat &src, float sigma)
{
	Mat padded = image_make_border(src);
	Mat ideal_kernel = ideal_high_kernel(padded, sigma);
	Mat result = frequency_filter(padded, ideal_kernel);
	return result;
}

Mat gaussian_low_pass_kernel(Mat scr, float sigma)
{
	Mat gaussianBlur(scr.size(), CV_32FC1);
	float d0 = 2 * sigma*sigma;
	for (int i = 0; i < scr.rows; i++) {
		for (int j = 0; j < scr.cols; j++) {
			float d = pow(float(i - scr.rows / 2), 2) + pow(float(j - scr.cols / 2), 2);
			gaussianBlur.at<float>(i, j) = expf(-d / d0);
		}
	}
	return gaussianBlur;
}

Mat gaussian_low_pass_filter(Mat &src, float d0)
{
	Mat padded = image_make_border(src);
	Mat gaussian_kernel = gaussian_low_pass_kernel(padded, d0);//ideal low pass filter
	Mat result = frequency_filter(padded, gaussian_kernel);
	return result;
}

Mat gaussian_high_pass_kernel(Mat scr, float sigma)
{
	Mat gaussianBlur(scr.size(), CV_32FC1);
	float d0 = 2 * sigma*sigma;
	for (int i = 0; i < scr.rows; i++) {
		for (int j = 0; j < scr.cols; j++) {
			float d = pow(float(i - scr.rows / 2), 2) + pow(float(j - scr.cols / 2), 2);
			gaussianBlur.at<float>(i, j) = 1 - expf(-d / d0);
		}
	}
	return gaussianBlur;
}

Mat gaussian_high_pass_filter(Mat &src, float d0)
{
	Mat padded = image_make_border(src);
	Mat gaussian_kernel = gaussian_high_pass_kernel(padded, d0);//ideal low pass filter
	Mat result = frequency_filter(padded, gaussian_kernel);
	return result;
}

Mat band_reject_kernel(Mat &scr, float sigma)
{
	Mat gaussianBlur(scr.size(), CV_32FC1);
	float d0 = sigma*sigma;
	for (int i = 0; i < scr.rows; i++) {
		for (int j = 0; j < scr.cols; j++) {
			float d = pow(float(i - scr.rows / 2), 2) + pow(float(j - scr.cols / 2), 2);
			gaussianBlur.at<float>(i, j) = 1 - expf((-1 * 0.5) * pow((d - d0) / (sqrt(d) * scr.cols), 2));
		}
	}
	return gaussianBlur;
}

Mat band_reject_pass_filter(Mat &src, float d0)
{
	Mat padded = image_make_border(src);
	Mat gaussian_kernel = band_reject_kernel(padded, d0);//ideal low pass filter
	Mat result = frequency_filter(padded, gaussian_kernel);
	return result;
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
	int height = 512, width = 512;
	unsigned char *fox = new unsigned char[height * width];
	unsigned char *foxNoise = new unsigned char[height * width];
	readImage(fox, "fox.raw", height, width);
	readImage(foxNoise, "fox_noise.raw", height, width);
	Mat foxImage(height, width, CV_8U, fox);
	Mat foxSpec = get<0>(dft(foxImage, height, width));
	Mat noisefoxImage(height, width, CV_8U, foxNoise);
	imwrite("foxNoiseOrigin.png", noisefoxImage);
	Mat foxNSpec = get<0>(dft(noisefoxImage, height, width));

	Mat ideal_low = ideal_low_pass_filter(foxImage, 5);
	Mat ideal_high = ideal_high_pass_filter(foxImage, 5);
	Mat Gaussian_low = gaussian_low_pass_filter(foxImage, 5);
	Mat Gaussian_high = gaussian_high_pass_filter(foxImage, 5);
	Mat bad_reject = band_reject_pass_filter(foxImage, 5);
	normalize(ideal_low, ideal_low, 0, 255, CV_MINMAX);
	imwrite("foxIdealLow.png", ideal_low);
	normalize(ideal_high, ideal_high, 0, 255, CV_MINMAX);
	imwrite("foxIdealHigh.png", ideal_high);
	normalize(Gaussian_low, Gaussian_low, 0, 255, CV_MINMAX);
	imwrite("foxGaussianLlow.png", Gaussian_low);
	normalize(Gaussian_high, Gaussian_high, 0, 255, CV_MINMAX);
	imwrite("foxGaussianHigh.png", Gaussian_high);
	normalize(bad_reject, bad_reject, 0, 255, CV_MINMAX);
	imwrite("foxBandReject.png", bad_reject);





	delete[] fox, foxNoise;
	



	system("pause");
	return 0;
}

