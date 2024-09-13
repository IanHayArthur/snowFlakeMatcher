#ifndef __UTILS_CC_INCLUDED__  
#define __UTILS_CC_INCLUDED__

#include "utils.h"
#include <vector>
#include <filesystem>
#include <algorithm>
#include <regex>
#include <utility>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// ================================================================================================
// Image Matching Utilities
// ================================================================================================

vector<filesystem::directory_entry> vectorizeFileDirectory(string path){
    vector<filesystem::directory_entry> returnVec;
    for (const auto& entry : filesystem::directory_iterator(path)) {
        returnVec.push_back(entry);
    }
    return returnVec;
}


void sortByDate(vector<filesystem::directory_entry>& images) {
    sort(images.begin(), images.end(),  
    [](const filesystem::directory_entry & a, const filesystem::directory_entry & b) {return a.last_write_time() < b.last_write_time();});
}

bool compareFiles(pair<string, filesystem::directory_entry> &a, pair<string, filesystem::directory_entry> &b )
{
    return stoi(a.first) < stoi(b.first); 
}

pair<map<string, vector<filesystem::directory_entry>>, map<string, vector<filesystem::directory_entry>>> intersectMaps( 
    map<string, vector<filesystem::directory_entry>> map1, map<string, vector<filesystem::directory_entry>> map2) {
    map<string, vector<filesystem::directory_entry>> map1Result;
    map<string, vector<filesystem::directory_entry>> map2Result;
    for (const auto& [key, value] : map1) {
        if (map2.find(key) != map2.end()) {
            map1Result[key] = value;
            map2Result[key] = map2[key];
        }
    }
    return {map1Result, map2Result};
}

vector<Point2f> toOutput(vector<filesystem::directory_entry> fileList) {
    vector<Point2f> vectorPairXY;
    for ( auto file : fileList) {
        string filename = file.path().string();
        vector<float> pairXY;
        regex rgx("X[0-9]*");
        regex rgy("Y[0-9]*");
        smatch matchX;
        smatch matchY;
        if(regex_search(filename, matchX, rgx)){
            string xCoordnate = matchX[0];
            pairXY.push_back(stof(xCoordnate.substr(1)));
        }
        if(regex_search(filename, matchY, rgy)){
            string yCoordnate = matchY[0];
            pairXY.push_back(stof(yCoordnate.substr(1)));
        }
        vectorPairXY.push_back(Point2f(pairXY[0], pairXY[1]));
    }
    return vectorPairXY;
}

double shortestDist(Point2f point, Vec3f line) {
    float a = line[0];
    float b = line[1];
    float c = line[2];
    float xp = point.x;
    float yp = point.y;

    double x = (1 / (pow(a, 2) + pow(b, 2))) * ((pow(b, 2) * xp) - (a * b * yp) - (a * c));
    double y = ((-a * x) / b) - (c / b);
    
    return sqrt(pow((yp - y), 2) + pow((xp-x), 2));
}

pair<vector<int>, vector<int>> findCorrespondIndicies(vector<Point2f> pts1, vector<Point2f> pts2, Mat fundamentalMatrix, pair<int, int> img1Size, pair<int, int> img2Size, 
                                   int distanceThresh) {
    vector<Vec3f> lines2; 
    vector<Vec3f> lines1; 
    computeCorrespondEpilines(pts1, 1, fundamentalMatrix, lines2);
    computeCorrespondEpilines(pts2, 2, fundamentalMatrix, lines1);

    vector<int> matchingIndicies2to1(pts1.size(), -99);
    vector<int> matchingIndicies1to2(pts2.size(), -99);
    double dist;
    for(int i = 0; i < lines2.size(); i ++) {
        for(int j = 0; j < pts2.size(); j++) {
            dist = shortestDist(pts2[j], lines2[i]);
            if(dist < distanceThresh) {
                matchingIndicies2to1[i] = j;
            }
        }
    }
    for(int i = 0; i < lines1.size(); i ++) {
        for(int j = 0; j < pts1.size(); j++) {
            dist = shortestDist(pts1[j], lines1[i]);
            if(dist < distanceThresh) {
                matchingIndicies1to2[i] = j;
            }
        }
    }
    return {matchingIndicies1to2, matchingIndicies2to1};
}


// ================================================================================================
// Fundamental Matrix Utilities
// ================================================================================================

map<string, vector<filesystem::directory_entry>> sortFileDirectory( vector<filesystem::directory_entry> fileList, 
                                                                    string regx1, string regx2, string regx3, string regx4) {
    map<string, vector<filesystem::directory_entry>> returnSorted;
    for(auto file : fileList){
        string filename = file.path().string();
        regex rgx1(regx1);
        regex rgx2(regx2);
        regex rgx3(regx3);
        regex rgx4(regx4);
        smatch match;
        if(regex_search(filename, match, rgx1)){
           returnSorted[regx1].push_back(file);
        }
        if(regex_search(filename, match, rgx2)){
           returnSorted[regx2].push_back(file);
        }
        if(regex_search(filename, match, rgx3)){
           returnSorted[regx3].push_back(file);
        }
        if(regex_search(filename, match, rgx4)){
           returnSorted[regx4].push_back(file);
        }
    }
    return returnSorted;
}

vector<vector<float>> readCSV(string fileName){
    ifstream inputFile(fileName);
    if(!inputFile.is_open()) {
        cerr << "Could not open file: " << fileName << "\n";
        exit(1);
    }
    string line;
    vector<vector<float>> resultVector;
    while(getline(inputFile, line)) {
        stringstream ss(line);
        string item;
        vector<float> row;
        while (std::getline(ss, item, ',')) {
            row.push_back(stof(item));
        }
        resultVector.push_back(row);
    }
    return resultVector;
}

vector<vector<float>> padVector(vector<vector<float>>& input, float padding) {
    for(auto& elem : input) {
        elem.push_back(padding);
    }
    return input;
}

Mat convertVectorToMat(vector<vector<float>> input) {
    Mat returnMat(int(input.size()), int(input[0].size()), CV_32F);
    for(int i = 0; i < input.size(); i++){
        for(int j = 0; j < input[j].size(); j++) {
            returnMat.at<float>(i, j) = input[i][(j+1) % 2];
        }
    }
    return returnMat;
}

#endif