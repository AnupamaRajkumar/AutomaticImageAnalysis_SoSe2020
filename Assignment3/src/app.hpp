#pragma once
#ifndef __APP_H_
#define __APP_H_

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace app {
	void app(Mat& templateImage, Mat& queryImage, vector<Scalar>& objList, Mat& params);
}

#endif
