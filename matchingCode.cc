#ifndef __MATCHINGCODE_CC_INCLUDED__  
#define __MATCHINGCODE_CC_INCLUDED__

#include "matchingCode.h"
#include "utils.h"
#include "fundamentalMatrix.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <regex>
#include <utility>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace chrono;

void matchPics(vector<filesystem::directory_entry> cam1Images, vector<filesystem::directory_entry> cam2Images, Mat fundamentalMatrix, string outputFile) {

    // For each entry in each cameras images, create a map that contains the image number, and the file
    // Then find the map intersection of the image numbers between each camera one for each camera, they have the same key values, 
    //      but have their own set of images
    map<string, vector<filesystem::directory_entry>> cam1Map;
    map<string, vector<filesystem::directory_entry>> cam2Map;
    map<string, vector<filesystem::directory_entry>>  intersectionMap1;
    map<string, vector<filesystem::directory_entry>>  intersectionMap2;


    for (const auto& entry : cam1Images){
        cam1Map[entry.path().stem().string().substr(5,6)].push_back(entry);
    }
    for (const auto& entry : cam2Images){
        cam2Map[entry.path().stem().string().substr(5,6)].push_back(entry);
    }
    
    tie(intersectionMap1, intersectionMap2) = intersectMaps(cam1Map, cam2Map);

    // for (auto [key, value] : intersectionMap1){
    //     cout << "File Number: " << key << " Number of Files: " << value.size() << "\n";
    // }
    // cout << "\n\n";
    // for (auto [key, value] : intersectionMap2){
    //     cout << "File Number: " << key << " Number of Files: " << value.size() << "\n";
    // }


    // For every intersection number, find the X and Y coordinates for that number on both camera
    // If there was an intersection at 000003
    // outputCam1 contains 911, 960 -- X and Y coordinates for the 000003 picture
    // outputCam2 contains 555, 1122 -- X and Y coordinates for the 000003 picture

    map<string, vector<Point2f>> outputCam1;
    map<string, vector<Point2f>> outputCam2;

    for (auto [key, value] : intersectionMap1) {
        outputCam1[key] = toOutput(value);
    }
    for (auto [key, value] : intersectionMap2) {
        outputCam2[key] = toOutput(value);
    }

    // for (auto pairs : outputCam1){
    //     for (auto pair : pairs){
    //         for (auto item : pair) {
    //             cout << item << " ";
    //         }   
    //     }
    //     cout << "\n";
    // }

    cout << "Number of OutputCam1: " << outputCam1.size() << "\n";
    cout << "Number of OutputCam2: " << outputCam2.size() << "\n";


    map<string, pair<vector<int>, vector<int>>> indiciesMap; // indicies1to2, indicies2to1

    for ( auto [key, value] : outputCam1 )  {
        indiciesMap[key] = findCorrespondIndicies(outputCam1[key], outputCam2[key], fundamentalMatrix, {1920,1200}, {2448,2048});
    }

    vector<pair<string, string>> pairedFiles;
    for ( auto [key, value] : indiciesMap) {
        for(int i = 0; i < value.first.size(); i++){
            if(value.first[i] >= 0) {
                if(value.second[value.first[i]] == i) {
                    pairedFiles.push_back({intersectionMap1[key][value.first[i]].path().string(), intersectionMap2[key][i].path().string()});
                }
            }
        }
    }

    ofstream outFile(outputFile);
    if (outFile.is_open()) {
        for (auto value : pairedFiles) {
            outFile << value.first << " , " << value.second << "\n";
        }
    }
    outFile.close();
}

#endif