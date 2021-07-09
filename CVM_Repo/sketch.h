#ifndef SKETCH_H
#define SKETCH_H

#include <QWidget>
#include <QLabel>
#include <QDir>
#include <QMessageBox>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "txt2vec.h"
#include "utm.h"

namespace Ui {
class sketch;
}

class sketch : public QWidget
{
    Q_OBJECT

public:
    explicit sketch(QWidget *parent = 0);
    ~sketch();

public:
    bool DrawTraj(cv::Mat _Img, vector<coordinate> _vecLatLon,
                  coordinate _LatLonLeftUpper, coordinate _LatLonRightBottom,
                  std::string _outputDir, int _UTMZone, bool _isSouth);

    bool SelectROI(int _width, int _interval,
                           QString _pathStr, string _postfix,
                           cv::Mat _Img,
                           vector<coordinate> _vecLatLon,
                           coordinate _LatLonLeftUpper,
                           coordinate _LatLonRightBottom,
                           int _UTMZone, bool _isSouth);

private slots:
    void on_loadTif_clicked();

    void on_loadGPS_clicked();

    void on_loadKml_clicked();

    void on_loadPanos_clicked();

    void on_loadOutputdir_clicked();

    void on_geneTraj_clicked();

    void on_geneSat_clicked();

    void on_geneGrd_clicked();

    void on_geneCSV_clicked();

    void on_ConfigConfirm_clicked();

    void on_loadAzi_clicked();

    void on_shiftDisplay_textChanged(const QString &arg1);

private:
    Ui::sketch *ui;
    QString tifFileName;
    QString GPSFileName;
    QString AziFileName;
    QString panosFolderName;
    QString outputFolderName;

    cv::Mat imgTif;
    std::vector<coordinate> vecLatLon;
    std::vector<coordinate> vecLatLonKML;
    std::vector<float> vecAzimuth;

    QString folderArea;
    int mSatLevel, mSatWidth, mImgInterval;
    bool isNorthAlign;
    float initialShift;
};

#endif // SKETCH_H
