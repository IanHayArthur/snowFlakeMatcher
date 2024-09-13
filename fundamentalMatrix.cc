#ifndef __FUNDAMENTALMATRIX_CC_INCLUDED__  
#define __FUNDAMENTALMATRIX_CC_INCLUDED__

#include "fundamentalMatrix.h"
#include "utils.h"
#include <filesystem>
#include <vector>
#include <sys/stat.h>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace cv;

Mat calculateFundementalMatrix(string filePath, string cam0wPt, string cam0iPt, string cam1wPt, string cam1iPt) {
    vector<vector<vector<float>>> objPoints0;
    vector<vector<vector<float>>> imgPoints0;
    vector<vector<vector<float>>> objPoints1;
    vector<vector<vector<float>>> imgPoints1;
    
    vector<filesystem::directory_entry> fileList;
    fileList = vectorizeFileDirectory(filePath);
    map<string, vector<filesystem::directory_entry>> sortedFiles;
    sortedFiles = sortFileDirectory(fileList, cam0wPt, cam0iPt, cam1wPt, cam1iPt);

    cout << sortedFiles[cam0wPt].size() << " " << sortedFiles[cam0iPt].size() << " " << 
            sortedFiles[cam1wPt].size() << " " <<  sortedFiles[cam1iPt].size() << "\n";
    for(int i = 0; i < sortedFiles[cam0wPt].size(); i++) {
        struct stat buf1;
        struct stat buf2;
        struct stat buf3;
        struct stat buf4;

        stat(sortedFiles[cam0wPt][i].path().string().c_str(), &buf1);
        stat(sortedFiles[cam0iPt][i].path().string().c_str(), &buf2);
        stat(sortedFiles[cam1wPt][i].path().string().c_str(), &buf3);
        stat(sortedFiles[cam1iPt][i].path().string().c_str(), &buf4);
        if(buf1.st_size > 20 && buf2.st_size > 90 && buf3.st_size > 20 && buf4.st_size > 90) {
            vector<vector<float>> w_Pt = readCSV(sortedFiles[cam0wPt][i].path().string());
            vector<vector<float>> i_Pt = readCSV(sortedFiles[cam0iPt][i].path().string());
            w_Pt = padVector(w_Pt, 0);
            objPoints0.push_back(w_Pt);
            imgPoints0.push_back(i_Pt);

            w_Pt = readCSV(sortedFiles[cam1wPt][i].path().string());
            i_Pt = readCSV(sortedFiles[cam1iPt][i].path().string());
            w_Pt = padVector(w_Pt, 0);
            objPoints1.push_back(w_Pt);
            imgPoints1.push_back(i_Pt);
        }
        
    }

    vector<vector<vector<float>>> objPoints;
    vector<vector<float>> imgPoints0_sel;
    vector<vector<float>> imgPoints1_sel;
    for(size_t i = 0; i < objPoints0.size(); i++){
        vector<vector<float>> intersection;
        vector<vector<float>> A;
        vector<vector<float>> B;
        A = objPoints0[i];
        B = objPoints1[i];
        sort(A.begin(), A.end());
        sort(B.begin(), B.end());
        set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(intersection));
        vector<size_t> indA;
        vector<size_t> indB;
        for(auto row : intersection) {
            indA.push_back(distance(A.begin(), find(A.begin(), A.end(), row)));
            indB.push_back(distance(B.begin(), find(B.begin(), B.end(), row)));
        }
        objPoints.push_back(intersection);

        for(size_t j = 0; j < indA.size(); j++) {
            imgPoints0_sel.push_back(imgPoints0[i][indA[j]]);
            imgPoints1_sel.push_back(imgPoints1[i][indB[j]]);
        }
    
    }
    Mat c0points = convertVectorToMat(imgPoints0_sel);
    Mat c1points = convertVectorToMat(imgPoints1_sel);

    Mat F = cv::findFundamentalMat(c0points, c1points, cv::FM_RANSAC, 0.05, 0.999);
    return F;
}

#endif