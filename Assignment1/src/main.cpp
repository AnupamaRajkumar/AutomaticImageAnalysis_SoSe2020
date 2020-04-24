#include <iostream>
#include <vector>

#include "given.hpp"
#include "yours.hpp"

using namespace std;
using namespace cv;
// TODO(student):
// Loads a test image and saves and shows the results of image processing
// Read the docs for OpenCV!
// https://docs.opencv.org/4.2.0/
int main(int argc, char** argv) {

    // TODO: declare some matrices for the images
    // https://docs.opencv.org/4.2.0/d3/d63/classcv_1_1Mat.html
	cout << "In main!!" << endl;

	//windows to display images
	const char* win_1 = "Original Image";
	const char* win_2 = "Smoothened Image";
	namedWindow(win_1);

    // TODO: load image
    // https://docs.opencv.org/4.2.0/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
	cout << "Load image" << endl;
	Mat inputImage = imread(argv[1]);
    std::string fname = "./data/test.jpg";

    // check if image is loaded successfully
    if (!inputImage.data){
      std::cerr << "ERROR: Cannot read file " << fname << std::endl;
      exit(-1);
    }

    // TODO: show input image
    // https://docs.opencv.org/master/d7/dfc/group__highgui.html#ga453d42fe4cb60e5723281a89973ee563
	cout << "Image loaded" << endl;
	cout << inputImage.dims<< endl;
	//display loaded image
	given::showImage(inputImage, win_1, 0);
	imwrite("original_input_image.png", inputImage);

    // TODO: call the function that handles the actual image processing
    // It's the function in yours.cpp called processImage
	Mat processedImage = yours::processImage(inputImage);	

    // TODO: save result
    // https://docs.opencv.org/master/d4/da8/group__imgcodecs.html#gabbc7ef1aa2edfaa87772f1202d67e0ce
	imwrite("smooth_binary_image.png", processedImage);

    // TODO: show result and wait for key
    // https://docs.opencv.org/master/d7/dfc/group__highgui.html#ga5628525ad33f52eab17feebcfba38bd7
	namedWindow(win_2);
	given::showImage(processedImage, win_2, 0);

	waitKey(0);

	return 0;
}
