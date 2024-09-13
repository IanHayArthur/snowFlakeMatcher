#ifndef __MAIN_CC_INCLUDED__  
#define __MAIN_CC_INCLUDED__

#include "matchingCode.h"
#include "utils.h"
#include "fundamentalMatrix.h"
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace chrono;

int main() {
    auto start = high_resolution_clock::now();

    string cam0Path = "cam1/28_cam1_cropped/sharp/";
    string cam1Path = "cam1/28_cam1_cropped/sharp/";
    string cam2Path = "cam2/28_cam2_cropped/sharp/";

    // Fundemental Matrix for the two camera being matched

    // Mat F01 = calculateFundementalMatrix("c01/", ".*Cam0.*wPt.*", ".*Cam0.*iPt.*", ".*Cam1.*wPt.*", ".*Cam1.*iPt.*");
    Mat F01 = (Mat_<float>(3, 3) << 7.51923156e-08,  2.81755226e-07, -3.79207973e-04,
                                    2.25512084e-07, -4.67941341e-08, -1.48810814e-03, 
                                    -4.38591473e-05,  1.81551149e-03,  1.00000000e+00);

    // Mat F02 = calculateFundementalMatrix("c02/", ".*Cam0.*wPt.*", ".*Cam0.*iPt.*", ".*Cam2.*wPt.*", ".*Cam2.*iPt.*");
    Mat F02 = (Mat_<float>(3, 3) << 0, 0, 0,
                                    0, 0, 0, 
                                    0, 0, 0);

    // Mat F12 = calculateFundementalMatrix("c12/", ".*Cam1.*wPt.*", ".*Cam1.*iPt.*", ".*Cam2.*wPt.*", ".*Cam2.*iPt.*");
    Mat F12 = (Mat_<float>(3, 3) << 2.21107922e-07, -6.04527898e-07,  5.62471057e-04,
                                    -4.52700820e-07, -1.50986002e-07, -2.35488590e-03,
                                    -7.58463124e-04,  5.45459685e-03,  1.00000000e+00);
 

    // Creates a vector of images in each of the cameras folders 

    vector<filesystem::directory_entry> cam0Images;
    vector<filesystem::directory_entry> cam1Images;                             
    vector<filesystem::directory_entry> cam2Images;

    cam0Images = vectorizeFileDirectory(cam0Path);
    cam1Images = vectorizeFileDirectory(cam1Path);
    cam2Images = vectorizeFileDirectory(cam2Path);

    cout << cam0Images.size() << " Flakes Cropped from Cam0 \n";
    cout << cam1Images.size() << " Flakes Cropped from Cam1 \n";
    cout << cam2Images.size() << " Flakes Cropped from Cam2 \n";

    //matchPics(cam0Images, cam1Images, F01, "output01.txt");
    //matchPics(cam0Images, cam2Images, F02, "output02.txt");
    matchPics(cam1Images, cam2Images, F12, "output12.txt");

    auto stop = high_resolution_clock::now();
    cout << "----execution took " << chrono::duration_cast<milliseconds>(stop - start).count() << " Milli Seconds----\n";

}

#endif 