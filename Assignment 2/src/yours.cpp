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
	cv::Mat kernel = cv::Mat::ones(3, 3, CV_8UC1);
	int maxVal = 255;
    if(src.channels() > 1)
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
   // get binary image (white foreground, black background)
	cv::threshold(tmp, dst, bin_thresh, maxVal, cv::THRESH_BINARY);
    // use erosion to get rid of small objects and break connections between leafs
    // use a 3x3 structuring element (cv::Mat::ones(3, 3, CV_8UC1))
	cv::erode(dst, dst, kernel);
	//given::showImage(dst);
}

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
	// translation invariance F(0) = 0
	fDescript.at<float>(0) = 0;
	
    // scale invariance F(i) = F(i)/|F(1)|
    // What if |F(1)| = 0?
	for (int row = 1; row < fDescript.rows ; row++)
	{
		if (fDescript.at<float>(1) > 0) {
			fDescript.at<float>(row) = fDescript.at<float>(row) / abs(fDescript.at<float>(1));
		}
		else
		{
			/*add a very small constant to F(1) if it is 0*/
			fDescript.at<float>(1) += pow(10, -3);
		}
	}
    // There are some useful OpenCV functions such as cartToPolar
	std::vector<cv::Mat> channels;
	split(fd, channels);

	cv::Mat angle;
	cv::Mat magnitude;
	//using cartoPolar to get magnitudes of the fourier descriptor
	cartToPolar(channels.at(0), channels.at(1), magnitude, angle, false);
	// Ensuring rotation invariance F = |F|
	for (int row = 1; row < fDescript.rows; row++)
	{
		fDescript.at<float>(row) = magnitude.at<float>(row);
	}
	
    // smaller sensitivity for details
    // This one is a bit tricky. How does your descriptor look like?
    // Where are the high frequencies and where are the negative indices?
	Mat descFiltrd	 = fDescript.clone();
	for (int row = 0; row < fDescript.rows; row++)
	{
		if (fDescript.at<float>(row) > 1000)
		{
			fDescript.at<float>(row) = 0;
			//row++;
		}
		else {
			//descFiltrd.push_back(fDescript.at<float>(row));
			descFiltrd.at<float>(row) = fDescript.at<float>(row);
			//do nothing
		}
	}
	//cout << "Fourier descriptor after normalization";
	return descFiltrd;
	//return fDescript;
}

//objects are assigned true (1)
//backgrounds are assigned false (0)
int yours::classifyFourierDescriptor(const cv::Mat& fd, const std::vector<cv::Mat>& class_templates, float thresh) {
    // loop over templates and find closest, return index
    // use cv::norm as a distance metric
	int index, cnt;
	for (auto& c : class_templates) {
		double dist = norm(fd, c, NORM_L2);
		cnt = 0;
		cout << dist << endl;
		if (dist > thresh) {
			index = cnt;
			return index;
		}
		cnt++;
	}	
}
