#include "txt2vec.h"

vector<coordinate> Txt2Vec(string txtPath)
{
        cout << "Ready to open file..." << endl;
        vector<coordinate> vecLatLon;
        ifstream file;
        file.open(txtPath, ifstream::in);
        if(!file)
        {
                cout<< "Cannot open file!" << endl;
                return vecLatLon;
        }

        string line;
        size_t mark = 0;
        size_t markTemp = 0;
        size_t markCnt = 0;
        size_t lineCnt = 0;

        while (!file.eof())
        {
                getline(file,line);
                lineCnt++;
                if(lineCnt >= 2)//Set the index of line which you want start at
                {
                        mark = line.find(' ',0);//Set the seperate marker here
                        markCnt++;
                        while (mark < line.size())
                        {
                                markTemp = line.find(' ', mark + 1);
                                markCnt++;
                                if(markCnt == 8)
                                {
                                         coordinate LatLon;
                                         LatLon.x = atof(line.substr(markTemp+1, 11).c_str());
                                         LatLon.y = atof(line.substr(markTemp+14, 11).c_str());
                                         vecLatLon.push_back(LatLon);
                                         break;
                                }
                                mark = markTemp;
                        }
                        markCnt = 0;
                }
        }

        file.close();
        cout << "File closed. " << endl;
        for(int i = 0; i < vecLatLon.size(); i++){
            cout << vecLatLon[i].x << endl;
        }
        return vecLatLon;
}

vector< coordinate > Kml2Vec(string kmlPath)
{
        cout << "Ready to open file..." << endl;
        vector<coordinate> vecLatLon;
        ifstream file;
        file.open(kmlPath, ifstream::in);
        if(!file)
        {
                cout<< "Cannot open file!" << endl;
                return vecLatLon;
        }

        string line;
        size_t lineCnt = 0;
        while (!file.eof())
        {
                getline(file,line);
                lineCnt++;
                if(lineCnt == 8)//Find the line which contains the LU and RB coordinates
                {
                        coordinate LU, RB;
                        LU.x = atof(line.substr(76, 18).c_str()); //(74, 18).c_str()); // (76, 18).c_str());<-this for BIT and (74, 18) for BITsmall
                        LU.y = atof(line.substr(57, 18).c_str()); //(57, 16).c_str()); // (57, 18).c_str());
                        RB.x = atof(line.substr(152, 18).c_str()); //(150, 18).c_str()); // (152, 18).c_str());
                        RB.y = atof(line.substr(133, 18).c_str()); //(131, 18).c_str()); // (133, 18).c_str());
                        vecLatLon.push_back(LU);
                        vecLatLon.push_back(RB);
                        break;
                }
        }

        file.close();
        cout << "File closed. " << endl;

        for(int i = 0; i < vecLatLon.size(); i++){
            cout << vecLatLon[i].x << endl;
        }

        return vecLatLon;
}

vector< float > Azimuth2Vec(string azimuthPath, int _index)
{
    cout << "Ready to open file..." << endl;
    vector<float> vecAzimuth;
    ifstream file;
    file.open(azimuthPath, ifstream::in);
    if(!file)
    {
        cout<< "Cannot open file!" << endl;
        return vecAzimuth;
    }

    string line;
    size_t mark = 0;
    size_t markTemp = 0;
    vector<int> vecMarkIndex;

    while (!file.eof())
    {
        getline(file,line);
        if(line.size() == 0) continue;

        //get mark index
        vector<int> vecMarkIndex;
        mark = line.find(',', 0);
        vecMarkIndex.push_back(mark);
        markTemp = mark;
        while(mark < line.size())
        {
            mark = line.find(',', markTemp + 1);
            vecMarkIndex.push_back(mark);
            markTemp = mark;
        }

        //get Azimuth
        float azimuth = atof(line.substr(vecMarkIndex[_index-1]+1, vecMarkIndex[_index]-vecMarkIndex[_index-1]-1).c_str());
        vecAzimuth.push_back(azimuth);
    }

    file.close();
    cout << "File closed. " << endl;
    return vecAzimuth;
}
