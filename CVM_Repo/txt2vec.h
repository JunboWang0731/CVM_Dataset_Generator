#ifndef TXT2VEC_H
#define TXT2VEC_H

#include "./util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

vector<coordinate> Txt2Vec(string txtPath);

vector<coordinate> Kml2Vec(string kmlPath);

vector<float> Azimuth2Vec(string azimuthPath, int _index);

#endif // TXT2VEC_H
