//
// Implement the required functions here.
//
#include "yours.hpp"
#include "given.hpp"


void yours::binarizeImage(const cv::Mat& src, cv::Mat& dst, int thresh) {

	cv::Mat dstGray, dstBin;
	double maxVal = 255;
	dstGray = src.clone();
	// TODO: convert image to gray scale
    // https://docs.opencv.org/4.2.0/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
	if (src.size() == dstGray.size()) {
		std::cout << "Converting to gray scale" << std::endl;
		cv::cvtColor(src, dstGray, cv::COLOR_BGR2GRAY);
	}
	
    // TODO: Binarize image
    // https://docs.opencv.org/4.2.0/d7/d1b/group__imgproc__misc.html#gae8a4a146d1ca78c626a53577199e9c57
	std::cout << " Converting to binary" << std::endl;
	cv::threshold(dstGray, dst, thresh, maxVal, cv::THRESH_BINARY);
}


// TODO: Implement processImage
// use smoothImage from given.hpp
//   (tests only work if you don't change the default value for size)
cv::Mat yours::processImage(const cv::Mat& image) {
	int size = 3;
	int thresh = 127;
	cv::Mat dst, smoothDst;
	dst = image.clone();
	smoothDst = dst.clone();
	binarizeImage(image, dst, thresh);
	given::smoothImage(dst, smoothDst, size);
	return smoothDst.clone();
}
