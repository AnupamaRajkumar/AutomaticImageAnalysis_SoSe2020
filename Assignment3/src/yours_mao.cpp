#include "yours.hpp"
#include "given.hpp"



Mat yours::binarizeGradientImage(Mat& src, double threshold){
    //    calculate gradient magnitude
    Mat binGradImg;
    Mat complGrad[2];
    split(src, complGrad);
    //    1st channel: gradients in x-direction
    //    2nd channel: gradients in y-direction
    magnitude(complGrad[0], complGrad[1], binGradImg);
    double maxVal;
    minMaxIdx(binGradImg, 0, &maxVal);
    for (int r = 0; r < binGradImg.rows; r++) {
        for (int c = 0; c < binGradImg.cols; c++) {
            if (binGradImg.at<float>(r,c) <= threshold*maxVal) {
                binGradImg.at<float>(r,c) = 0;
            }
            else{
                binGradImg.at<float>(r,c) = 1;
            }
            
            
        }
    }
    
    return binGradImg;
}

Mat yours::visualizeHoughSpace(vector< vector<Mat> >& houghSpace){
    
    //  Creates a single image from the 4D Hough space
    //  mat : 2D ; vector<Mat> : 3D ; vector< vector<Mat> > : 4D
    Mat houghImage(houghSpace.at(0).at(0).rows, houghSpace.at(0).at(0).cols, CV_32FC1);
    
    //    vote
    for (auto i : houghSpace){
        for (auto j : i){
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
    
    Mat binEdge = templ[0].clone();
    Mat complGrad = templ[1].clone();
    
    
    // Binary object mask and complex object gradients are resized
    // Binary object mask and complex object gradients are rotated
    binEdge = given::rotateAndScale(binEdge,angle,scale);
    complGrad = given::rotateAndScale(complGrad,angle,scale);

    float sum_Grad = 0.0;
    for(int i=0; i<complGrad.rows; i++){
        for(int j=0; j<complGrad.cols; j++){
            float x = complGrad.at<Vec2f>(i,j)[0];
            float y = complGrad.at<Vec2f>(i,j)[1];
            //Complex gradients are rotated by applying a phase shift
            complGrad.at<Vec2f>(i,j)[0] = cos(angle) * x - sin(angle) * y;
            complGrad.at<Vec2f>(i,j)[1] = sin(angle) * x + cos(angle) * y;
            //Normalization
            sum_Grad += sqrt(pow(complGrad.at<Vec2f>(i,j)[0], 2) + pow(complGrad.at<Vec2f>(i,j)[1], 2));
        }
    }
    
    complGrad /= sum_Grad;
    
    // Copy the object mask into a larger matrix ;
    Mat T = Mat::zeros(fftMask.rows, fftMask.cols, CV_32FC2);
    for(int i=0; i<complGrad.rows; i++){
        for(int j=0; j<complGrad.cols; j++){
            T.at<Vec2f>(i,j)[0] = complGrad.at<Vec2f>(i,j)[0] * binEdge.at<float>(i,j);//T = OI*OB
            T.at<Vec2f>(i,j)[1] = complGrad.at<Vec2f>(i,j)[1] * binEdge.at<float>(i,j);//T = OI*OB
        }
    }
    
    // Centre the filter (the centre is shifted to coordinates (0,0) )
    given::circShift(T, T, -binEdge.cols/2, -binEdge.rows/2);
    
    // Transfer filter and image to the frequency domain
    dft(T, fftMask, DFT_COMPLEX_OUTPUT);
}


vector<vector<Mat> > yours::generalHough(Mat& gradImage, vector<Mat>& templ, double scaleSteps, double* scaleRange, double angleSteps, double* angleRange){
    
    vector< vector<Mat> > hough;
    int row = gradImage.rows;
    int col = gradImage.cols;
    
    Mat edge = templ[0];
    
    Mat GradXY[2];
    split(templ[1], GradXY);
    Mat GradX = GradXY[0];
    Mat GradY = GradXY[1];
    
    float scaleInterval = (scaleRange[1] - scaleRange[0]) / (scaleSteps-1);
    float angleInterval = (angleRange[1] - angleRange[0]) / angleSteps;
    
    Mat fftGradImage;
    
    dft(gradImage, fftGradImage, DFT_COMPLEX_OUTPUT, 0);
    
    Mat fftMask = Mat::zeros(row, col, CV_32FC2);
    
    for(int i=0; i<scaleSteps; i++){
        vector<Mat> angelvector;
        for(int j=0; j<angleSteps-1; j++){
            makeFFTObjectMask(templ, scaleRange[0] + i*scaleInterval, angleRange[0] + j*angleInterval, fftMask);
            
            Mat ImgAndCCFilterSpectrum = Mat::zeros(row, col, CV_32FC1);
 
            mulSpectrums(fftGradImage, fftMask, ImgAndCCFilterSpectrum, 0, true);
            dft(ImgAndCCFilterSpectrum, ImgAndCCFilterSpectrum, DFT_INVERSE | DFT_SCALE, 0);
            Mat complHoughReIm[2];
            split(ImgAndCCFilterSpectrum,complHoughReIm);
            Mat Abs_Re = abs(complHoughReIm[0]);
            angelvector.push_back(Abs_Re);
        }
        hough.push_back(angelvector);
        
    }
    return hough;
}


