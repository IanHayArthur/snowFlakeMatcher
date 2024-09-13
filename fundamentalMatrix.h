#ifndef __FUNDAMENTALMATRIX_H_INCLUDED__  
#define __FUNDAMENTALMATRIX_H_INCLUDED__

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat calculateFundementalMatrix(string filePath, string cam0wPt, string cam0iPt, string cam1wPt, string cam1iPt);

#endif