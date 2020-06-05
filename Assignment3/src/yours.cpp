#include "yours.hpp"
#include "given.hpp"

using namespace std;
using namespace cv;


Mat yours::visualizeHoughSpace(vector< vector<Mat> >& houghSpace){
	//  Creates a single image from the 4D Hough space
	//  mat : 2D ; vector<Mat> : 3D ; vector< vector<Mat> > : 4D
	Mat houghImage(houghSpace.at(0).at(0).rows, houghSpace.at(0).at(0).cols, CV_32FC1);

	//    vote
	for (auto i : houghSpace) {
		for (auto j : i) {
			for (int r = 0; r < j.rows; ++r) {
				for (int c = 0; c < j.cols; ++c) {
					houghImage.at<float>(r, c) += j.at<float>(r, c);
				}
			}
		}
	}
	return houghImage;
}


void yours::makeFFTObjectMask(vector<Mat>& templ, double scale, double angle, Mat& fftMask){

	Mat GradTempl, BinTempl, T;
	BinTempl = templ[0].clone();
	GradTempl = templ[1].clone();

	//step1: scale and rotate the binary and complex templates
	GradTempl = given::rotateAndScale(GradTempl, angle, scale);			//Oi
	BinTempl = given::rotateAndScale(BinTempl, angle, scale);			//Ob

	//normalize to retain scale invariance
	double normFac = 0.;
	double val = 0.;
	for (int row = 0; row < GradTempl.rows; row++) {
		for (int col = 0; col < GradTempl.cols; col++) {
			val = sqrt(pow(GradTempl.at<Vec2f>(row, col)[0], 2) + pow(GradTempl.at<Vec2f>(row, col)[1], 2));
			normFac += val;
		}
	}

	GradTempl /= normFac;

	//Calculate T = Oi*Ob by matrix multiplication
	T = Mat::zeros(fftMask.rows, fftMask.cols, CV_32FC2);
	for (int row = 0; row < GradTempl.rows; row++) {
		for (int col = 0; col < GradTempl.cols; col++) {
			T.at<Vec2f>(row, col)[0] = GradTempl.at<Vec2f>(row, col)[0] * BinTempl.at<float>(row, col);//T = OI*OB
			T.at<Vec2f>(row, col)[1] = GradTempl.at<Vec2f>(row, col)[1] * BinTempl.at<float>(row, col);//T = OI*OB
		}
	}

	//circshift
	given::circShift(T, T, -BinTempl.cols/2, -BinTempl.rows/2);

	//DFT
	dft(T, fftMask, DFT_COMPLEX_OUTPUT);
}



vector<vector<Mat>> yours::generalHough(Mat& gradImage, vector<Mat>& templ, double scaleSteps, double* scaleRange, double angleSteps, double* angleRange){

	Mat fftMask;
	vector<vector<Mat>> result;
	double scale, angle;
	scale = 0.;
	angle = 0.;
	float sInterval = (scaleRange[1] - scaleRange[0]) / (scaleSteps - 1);
	float aInterval = (angleRange[1] - angleRange[0]) / angleSteps;
	//convert image from time to frequency domain
	Mat fftGradImage;
	dft(gradImage, fftGradImage, DFT_COMPLEX_OUTPUT, 0);
	fftMask = Mat::zeros(gradImage.size(), CV_32FC2);
	//for each step and angle parameter
	for (int i = 0; i < scaleSteps; i++) {
		vector<Mat> angelvector;
		for (int j = 0; j < angleSteps - 1; j++) {
			makeFFTObjectMask(templ, scaleRange[0] + i * sInterval, angleRange[0] + j * aInterval, fftMask);
			Mat ImgAndCCFilterSpectrum = Mat::zeros(gradImage.size(), CV_32FC1);
			mulSpectrums(fftGradImage, fftMask, ImgAndCCFilterSpectrum, 0, true);
			dft(ImgAndCCFilterSpectrum, ImgAndCCFilterSpectrum, DFT_INVERSE | DFT_SCALE, 0);
			Mat complHoughReIm[2];
			split(ImgAndCCFilterSpectrum, complHoughReIm);
			Mat Abs_Re = abs(complHoughReIm[0]);
			angelvector.push_back(Abs_Re);
		}
		result.push_back(angelvector);

	}
	return result;
}



Mat yours::binarizeGradientImage(Mat& src, double threshold){
	//    calculate gradient magnitude
	Mat binGradImg;
	std::vector<cv::Mat> complGrad;
	split(src, complGrad);
	binGradImg = complGrad[0].clone();
	//    1st channel: gradients in x-direction
	//    2nd channel: gradients in y-direction
	magnitude(complGrad.at(0), complGrad.at(1), binGradImg);
	double maxVal;
	minMaxIdx(binGradImg, 0, &maxVal, 0, 0);
	      cout<<"maxvalue="<<maxVal<<endl;
	for (int r = 0; r < binGradImg.rows; r++) {					
		for (int c = 0; c < binGradImg.cols; c++) {					
			if (binGradImg.at<float>(r, c) <= threshold * maxVal) {
				binGradImg.at<float>(r, c) = 0;
			}
		}
	}
	imshow("Binarised Image", binGradImg);
	waitKey(0);
	return binGradImg;
}
