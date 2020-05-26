//
// Implement the required functions here.
//
#include "yours.hpp"
#include "given.hpp"
#include<vector>

using namespace cv;
using namespace std;

void yours::preprocessImage(cv::Mat& src, cv::Mat& dst, int bin_thresh, int n_erosions) {
    cv::Mat tmp = src;
	//cv::Mat dstBin = dst;
	cv::Mat kernel = cv::Mat::ones(3, 3, CV_8UC1)*255;

	int maxVal = 255;
    if(src.channels() > 1)
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
   // get binary image (white foreground, black background)
	cv::threshold(tmp, dst, bin_thresh, maxVal, cv::THRESH_BINARY_INV);
    // use erosion to get rid of small objects and break connections between leafs
    // use a 3x3 structuring element (cv::Mat::ones(3, 3, CV_8UC1))
	//cv::erode(dst, dst, kernel, Point(-1, -1), n_erosions);
	cv::erode(dst, dst, kernel, Point(-1, -1), n_erosions);
	//bitwise_not(dst, dst);
	//given::showImage(dst);
}

/*void yours::preprocessImage(cv::Mat &src, cv::Mat &dst, int bin_thresh, int n_erosions)
{

	cv::Mat tmp = src;
	if (src.channels() > 1)
		cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);

	// get binary image (white foreground, black background)
	cv::threshold(tmp, dst, 128, 255, cv::THRESH_BINARY_INV);
	//    given::showImage(dst);

	// use erosion to get rid of small objects and break connections between leafs
	// use a 3x3 structuring element (cv::Mat::ones(3, 3, CV_8UC1))
	cv::Mat element = cv::Mat::ones(3, 3, dst.type()) * 255;
	cv::erode(dst, dst, element, cv::Point(-1, -1), n_erosions);
}*/

cv::Mat yours::getFourierDescriptor(const cv::Mat& contour) {
    // convert the countour in a adequate format and make the discrete fourier transform
    // use OpenCVs implementation of the DFT
	cv::Mat padded;
	//convert contour to 32 bits, 2 channel
	contour.convertTo(padded, CV_32FC2);
	cv::Mat complexInput = contour.clone();
	dft(padded, complexInput);
	return complexInput;
}

cv::Mat yours::normalizeFourierDescriptor(const cv::Mat& fd, int n) {
    
	cv::Mat fDescript = fd.clone();
	//convert to 1 channel from 2 channels
	fDescript = fd.reshape(1);
	//resize to the decriptor_length
	fDescript.resize(n);
	//cout << fDescript.rows <<"x"<< fDescript.cols<<endl;
	// translation invariance F(0) = 0
	fDescript.at<float>(0,0) = 0.;
	fDescript.at<float>(0,1)= 0.;
	//cout << fDescript.size() << endl;
	//ensure an even descriptor length
	if (n % 2 != 0) {
		cout << "Ensure an even descriptor length. Exiting..." << endl;
		exit(-1);
	}
	// What if |F(1)| = 0?
	float eps = pow(10, -5);
	fDescript.at<float>(1,0) += eps;
	fDescript.at<float>(1,1) += eps;
    // scale invariance F(i) = F(i)/|F(1)|
	double Scale = sqrt(pow(fDescript.at<float>(1, 0), 2) + pow(fDescript.at<float>(1, 1), 2));
	for (int row = 1; row < fDescript.rows; row++) {
		//fDescript.at<float>(row, 0) = fDescript.at<float>(row, 0) / abs(fDescript.at<float>(1, 0));
		//fDescript.at<float>(row, 1) = fDescript.at<float>(row, 1) / abs(fDescript.at<float>(1, 1));
		fDescript.at<float>(row) = fDescript.at<float>(row) / Scale;
	}
	//cout << "fDescript:" << fDescript << endl;
    // There are some useful OpenCV functions such as cartToPolar

	std::vector<cv::Mat> channels;
	split(fd, channels);

	cv::Mat angle;
	cv::Mat magnitude;

	//using cartoPolar to get magnitudes of the fourier descriptor
	cartToPolar(channels.at(0), channels.at(1), magnitude, angle, false);
	// Ensuring rotation invariance F = |F|
	Mat descFiltrd = cv::Mat::zeros(fDescript.rows, 1, CV_32FC1);
	for (int row = 1; row < descFiltrd.rows; row++) {
		descFiltrd.at<float>(row) = magnitude.at<float>(row);
	}
    // Smaller sensitivity for details
    // This one is a bit tricky. How does your descriptor look like?
    // Where are the high frequencies and where are the negative indices
	// The values in fourier series are positive from 0 to N/2 and  
	// negative from N/2 to N because it is a periodic signal
	for (int row = descFiltrd.rows/2; row < descFiltrd.rows; row++) {
		descFiltrd.at<float>(row) = 0;
	}
	return descFiltrd;
}

//objects are assigned true (1)
//backgrounds are assigned false (0)
int yours::classifyFourierDescriptor(const cv::Mat& fd, const std::vector<cv::Mat>& class_templates, float thresh) {
    // loop over templates and find closest, return index
    // use cv::norm as a distance metric
	int index, cnt, mindist;
	index = 0;
	cnt = -1;
	mindist = pow(10, 5);
	//cout << fd.size() << endl;
	for (auto& c : class_templates) {
		//cout << c.size() << endl;
		double dist = norm(fd, c, NORM_L2);
		//cout << dist << endl;
		if (dist < thresh && dist < mindist) {
			mindist = dist;
			cnt = index;
		}
		index++;
	}
	return cnt;
}


