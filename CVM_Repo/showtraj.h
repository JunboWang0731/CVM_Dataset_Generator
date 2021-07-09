#ifndef SHOWTRAJ_H
#define SHOWTRAJ_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include "txt2vec.h"
#include "utm.h"

bool DrawTraj(cv::Mat _Img, vector<coordinate> _vecLatLon,
              coordinate _LatLonLeftUpper, coordinate _LatLonRightBottom,
              std::string _outputDir, int _UTMZone, bool _isSouth);

#endif // SHOWTRAJ_H
