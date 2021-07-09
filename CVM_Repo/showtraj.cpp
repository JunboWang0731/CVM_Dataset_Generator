#include "showtraj.h"

using namespace std;
using namespace cv;

bool DrawTraj(cv::Mat _Img, vector<coordinate> _vecLatLon,
              coordinate _LatLonLeftUpper, coordinate _LatLonRightBottom,
              std::string _outputDir, int _UTMZone, bool _isSouth)
{
    if(_vecLatLon.empty()){
        return false;
    }
    cv::Mat ImgCopy = _Img.clone();
    for(int i = 0; i < _vecLatLon.size(); i++)
    {
        cout << "Position counts: " << i << endl;
        coordinate imgUV;
        imgUV.x = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).x;
        imgUV.y = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).y;
        if(imgUV.x < _Img.cols && imgUV.y < _Img.rows)
        {
            Point p(imgUV.x, imgUV.y);
            circle(ImgCopy, p, 10, Scalar(0, 255, 0), -1);
        }
        else
            cout << "This point do not exist in the satellite image!" << endl;
    }
    imwrite(_outputDir + "/Trajectory.jpg", ImgCopy);
    cout << "Trajectory has been saved!" << endl;
    return true;
}

//int main(int argc, char* argv[])
//{
//        if(argc < 4)
//        {
//                cout << "Please enter the txt file path, satellite image path and kml file path" << endl;
//                return 0;
//        }

//        vector<coordinate> vecLatLon = Txt2Vec(argv[1]);
//        if(vecLatLon.size() == 0)
//        {
//                cout << "No GPS information under this path!" << endl;
//                return 0;
//        }

//        Mat Img = imread(argv[2]);
//        if(Img.size == 0)
//        {
//                cout << "No Image under this path!" << endl;
//                return 0;
//        }

//        vector<coordinate> vecLatLonKML = Kml2Vec(argv[3]);
//        if(vecLatLon.size() == 0)
//        {
//                cout << "No kml information under this path!" << endl;
//                return 0;
//        }

//        coordinate resolution = {0.0, 0.0};
//        coordinate LonLatLeftUpper = {vecLatLonKML.front().y, vecLatLonKML.front().x};
//        coordinate LonLatRightBottom = {vecLatLonKML.back().y, vecLatLonKML.back().x};
//        //The UTM zone belongs to Beijing area
//        int UTMZone = 50;
//        bool isSouth = 0;

//        DrawTraj(Img, vecLatLon, LonLatLeftUpper, LonLatRightBottom, UTMZone, isSouth);

//        return 0;
//}
