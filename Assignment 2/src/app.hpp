#pragma once
//
// Do not change this file.
//


//#include <iostream>
//#include <opencv2/opencv.hpp>
#include "given.hpp"
#include "yours.hpp"

struct input {
	std::string path;
	cv::Mat img;
	int bin_thresh;
	int n_erosions;
};

namespace app {

	void app(std::vector<input> templates, input query, int descriptor_length, float detection_thresh);

}


