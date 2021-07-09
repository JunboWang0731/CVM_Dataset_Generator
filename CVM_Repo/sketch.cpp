#include "sketch.h"
#include "ui_sketch.h"
#include <qfiledialog.h>
#include <opencv.hpp>

sketch::sketch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sketch)
{
    ui->setupUi(this);
}

sketch::~sketch()
{
    delete ui;
}

bool sketch::DrawTraj(cv::Mat _Img, std::vector<coordinate> _vecLatLon,
              coordinate _LatLonLeftUpper, coordinate _LatLonRightBottom,
              std::string _outputDir, int _UTMZone, bool _isSouth)
{
    if(_vecLatLon.empty()){
        return false;
    }
    cv::Mat ImgCopy = _Img.clone();
    for(int i = 0; i < _vecLatLon.size(); i++)
    {
        std::cout << "Position counts: " << i << std::endl;
        coordinate imgUV;
        imgUV.x = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).x;
        imgUV.y = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).y;
        if(imgUV.x < _Img.cols && imgUV.y < _Img.rows)
        {
            cv::Point p(imgUV.x, imgUV.y);
            cv::circle(ImgCopy, p, 10, cv::Scalar(0, 255, 0), -1);
        }
        else
            std::cout << "This point do not exist in the satellite image!" << std::endl;
    }
    cv::imwrite(_outputDir + "/Trajectory.jpg", ImgCopy);
    std::cout << _outputDir << "/Trajectory.jpg has been saved!" << std::endl;
    return true;
}

bool sketch::SelectROI(int _width, int _interval, QString _pathStr, string _postfix, cv::Mat _Img, vector<coordinate> _vecLatLon, coordinate _LatLonLeftUpper, coordinate _LatLonRightBottom, int _UTMZone, bool _isSouth)
{
    if(_vecLatLon.empty()){
        return false;
    }
    else{
        QDir dir(_pathStr+"/sat"+"/"+ui->SatLevel->text());
        if(!dir.exists()){
            QDir().mkpath(_pathStr+"/sat"+"/"+ui->SatLevel->text());
        }
        qDebug("%s", qPrintable(_pathStr+"/sat"+"/"+ui->SatLevel->text()));
        for(int i = 0; i < _vecLatLon.size(); i+=_interval)
        {
            coordinate imgUV;
            imgUV.x = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).x;
            imgUV.y = LatLonToImgUV(_Img, _LatLonLeftUpper, _LatLonRightBottom, _UTMZone, _isSouth, _vecLatLon.at(i)).y;
            if(imgUV.x - _width/2 > 0 && imgUV.y - _width/2 > 0 && imgUV.x + _width/2 < _Img.cols && imgUV.y + _width/2 < _Img.rows)
            {
                cv::Mat roi = _Img(cv::Rect(imgUV.x -_width/2, imgUV.y -_width/2, _width, _width));
                char index[10];
                sprintf(index, "%06d", i);
                std::cout << _pathStr.toStdString() + "/"+std::to_string(mSatLevel) + "/sat/" + index + _postfix << std::endl;
                cv::imwrite(_pathStr.toStdString() + "/sat" + "/"+ui->SatLevel->text().toStdString() + "/" + index + _postfix, roi);
            }
            else
                std::cout << "This ROI do not exist in the satellite image!" << std::endl;
        }
        return true;
    }
}

void sketch::on_loadTif_clicked()
{
    tifFileName = QFileDialog::getOpenFileName(this, "Open Tif File", "", "tif file(*.tif)");
    if(!tifFileName.isEmpty()){
        ui->loadTif->setStyleSheet("background-color: rgb(0,255,0)");
        std::string imgName = tifFileName.toStdString();
        imgTif = cv::imread(imgName);
        cv::Mat imgRGB = imgTif.clone();
        QImage Img;
        cv::cvtColor(imgTif, imgRGB, CV_BGR2RGB);//颜色空间转换
        Img = QImage((const uchar*)(imgRGB.data), imgRGB.cols, imgRGB.rows, imgRGB.cols * imgRGB.channels(), QImage::Format_RGB888);

        ui->QlabelImg->setScaledContents(true);
        ui->QlabelImg->setPixmap(QPixmap::fromImage(Img));
    }
    else{
        ui->loadTif->setStyleSheet("background-color: rgb(255,0,0)");
    }
    qDebug("tif file path = %s", qPrintable(tifFileName));

}

void sketch::on_loadGPS_clicked()
{
    GPSFileName = QFileDialog::getOpenFileName(this, "Open GPS coordinate File", "", "*.txt");
    if(!GPSFileName.isEmpty()){
        ui->loadGPS->setStyleSheet("background-color: rgb(0,255,0)");
        vecLatLon.clear();
        vecLatLon = Txt2Vec(GPSFileName.toStdString());
    }
    else{
        ui->loadGPS->setStyleSheet("background-color: rgb(255,0,0)");
    }
    qDebug("GPS file path = %s", qPrintable(GPSFileName));
}

void sketch::on_loadKml_clicked()
{
    // clean the vecLatLonKMML
    vecLatLonKML.clear();

    coordinate LU, RB;
    LU.x = ui->LULat->text().toDouble();
    LU.y = ui->LULon->text().toDouble();
    RB.x = ui->RBLat->text().toDouble();
    RB.y = ui->RBLon->text().toDouble();

    if(LU.x == 0 || LU.y == 0 || RB.x == 0 || RB.y == 0){
        QMessageBox::warning(NULL, "Kml setting", "Invalid kml input, reset it again!", QMessageBox::Yes | QMessageBox::Yes);
        ui->loadKml->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }

    cout << "LU.Lat: " << LU.x << endl;
    cout << "LU.Lon: " << LU.y << endl;
    cout << "RB.Lat: " << RB.x << endl;
    cout << "RB.Lon: " << RB.y << endl;
    vecLatLonKML.push_back(LU);
    vecLatLonKML.push_back(RB);
    ui->loadKml->setStyleSheet("background-color: rgb(0,255,0)");
    return;
}

void sketch::on_loadPanos_clicked()
{
    panosFolderName = QFileDialog::getExistingDirectory(this, "choose src Directory", "");
    if(!panosFolderName.isEmpty()){
        ui->loadPanos->setStyleSheet("background-color: rgb(0,255,0)");

        std::vector<cv::String> fileNames;
        cv::glob(panosFolderName.toStdString() + "/", fileNames);

        cv::Mat imgPano = cv::imread(fileNames[1], 1);

        initialShift = ui->shiftDisplay->text().toInt();
        int shift = int(initialShift * imgPano.cols / 360);
        cv::Mat resultImg = cv::Mat(imgPano.rows, imgPano.cols, CV_8UC3, cv::Scalar::all(0));
        cv::Mat part1 = imgPano(cv::Rect(0, 0, imgPano.cols - shift , imgPano.rows));
        cv::Mat part2 = imgPano(cv::Rect(imgPano.cols - shift, 0, shift, imgPano.rows));
        cv::Mat ROI_1 = resultImg(cv::Rect(0, 0, shift, imgPano.rows));
        cv::Mat ROI_2 = resultImg(cv::Rect(shift, 0, imgPano.cols - shift, imgPano.rows));
        part1.copyTo(ROI_2);
        part2.copyTo(ROI_1);

        cv::Mat imgRGB = resultImg.clone();
        QImage Img;
        cv::cvtColor(resultImg, imgRGB, CV_BGR2RGB);//颜色空间转换
        Img = QImage((const uchar*)(imgRGB.data), imgRGB.cols, imgRGB.rows, imgRGB.cols * imgRGB.channels(), QImage::Format_RGB888);

        ui->labelPano->setScaledContents(true);
        ui->labelPano->setPixmap(QPixmap::fromImage(Img));
    }
    else{
        ui->loadPanos->setStyleSheet("background-color: rgb(255,0,0)");
    }
    qDebug("panos folder path = %s", qPrintable(panosFolderName));
}

void sketch::on_loadOutputdir_clicked()
{
    outputFolderName = QFileDialog::getExistingDirectory(this, "choose output Directory", "");
    if(!outputFolderName.isEmpty()){
        ui->loadOutputdir->setStyleSheet("background-color: rgb(0,255,0)");
    }
    else{
        ui->loadOutputdir->setStyleSheet("background-color: rgb(255,0,0)");
    }
    qDebug("output folder path = %s", qPrintable(outputFolderName));
}

void sketch::on_geneTraj_clicked()
{
    if(tifFileName.isEmpty() || vecLatLon.size() == 0
            || vecLatLonKML.size() == 0 || outputFolderName.isEmpty()){
        std::cout << "Lack of required files, please do check!" << std::endl;
        ui->geneTraj->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }

    QDir dir(outputFolderName + "/" + ui->Area->text());
    if(!dir.exists()){
        QDir().mkpath(outputFolderName + "/" + ui->Area->text());
    }

    coordinate LonLatLeftUpper = {vecLatLonKML.front().y, vecLatLonKML.front().x};
    coordinate LonLatRightBottom = {vecLatLonKML.back().y, vecLatLonKML.back().x};
    if(sketch::DrawTraj(imgTif, vecLatLon, LonLatLeftUpper, LonLatRightBottom, (outputFolderName+"/"+ui->Area->text()).toStdString(), 50, false)){

        std::string imgName = (outputFolderName + "/" + ui->Area->text() + "/Trajectory.jpg").toStdString();
        cv::Mat imgTraj = cv::imread(imgName);
        cv::Mat imgRGB = imgTraj.clone();
        QImage Img;
        cv::cvtColor(imgTraj, imgRGB, CV_BGR2RGB);//颜色空间转换
        Img = QImage((const uchar*)(imgRGB.data), imgRGB.cols, imgRGB.rows, imgRGB.cols * imgRGB.channels(), QImage::Format_RGB888);

        ui->QlabelImg->setScaledContents(true);
        ui->QlabelImg->setPixmap(QPixmap::fromImage(Img));

        ui->geneTraj->setStyleSheet("background-color: rgb(0,255,0)");
    }
}

void sketch::on_geneSat_clicked()
{
    if(tifFileName.isEmpty() || vecLatLon.size() == 0
            || vecLatLonKML.size() == 0 || outputFolderName.isEmpty()){
        std::cout << "Lack of required files, please do check!" << std::endl;
        ui->geneSat->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }
    coordinate LonLatLeftUpper = {vecLatLonKML.front().y, vecLatLonKML.front().x};
    coordinate LonLatRightBottom = {vecLatLonKML.back().y, vecLatLonKML.back().x};
    if(sketch::SelectROI(mSatWidth, mImgInterval, outputFolderName+"/"+ui->Area->text(), ".jpg", imgTif, vecLatLon, LonLatLeftUpper, LonLatRightBottom, 50, false)){
        ui->geneSat->setStyleSheet("background-color: rgb(0,255,0)");
    }
}

void sketch::on_geneGrd_clicked()
{
    if(tifFileName.isEmpty() || vecLatLon.size() == 0
            || vecLatLonKML.size() == 0 || outputFolderName.isEmpty()){
        std::cout << "Lack of required files, please do check!" << std::endl;
        ui->geneGrd->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }

    cv::String savedFileName;
    std::vector<cv::String> fileNames;
    cv::Mat srcImg, dstImg;

    if(isNorthAlign){
        QDir dir(outputFolderName + "/" + ui->Area->text() + "/grd/Aligned");
        if(!dir.exists()){
            QDir().mkpath(outputFolderName + "/" + ui->Area->text() + "/grd/Aligned");
        }
    }
    else{
        QDir dir(outputFolderName + "/" + ui->Area->text() + "/grd/notAligned");
        if(!dir.exists()){
            QDir().mkpath(outputFolderName + "/" + ui->Area->text() + "/grd/notAligned");
        }
    }

    cv::glob(panosFolderName.toStdString() + "/", fileNames);
    if(fileNames.size() != 0){
        for(int i = 0; i < fileNames.size(); i+=mImgInterval){
            srcImg = cv::imread(fileNames[i], 1);

            if(isNorthAlign){
                // do initial shift firstly
                int initialshift = int(initialShift * srcImg.cols / 360);
                cv::Mat resultImg1 = cv::Mat(srcImg.rows, srcImg.cols, CV_8UC3, cv::Scalar::all(0));
                cv::Mat part1_1 = srcImg(cv::Rect(0, 0, srcImg.cols - initialshift , srcImg.rows));
                cv::Mat part2_1 = srcImg(cv::Rect(srcImg.cols - initialshift, 0, initialshift, srcImg.rows));
                cv::Mat ROI_1_1 = resultImg1(cv::Rect(0, 0, initialshift, srcImg.rows));
                cv::Mat ROI_2_1 = resultImg1(cv::Rect(initialshift, 0, srcImg.cols - initialshift, srcImg.rows));
                part1_1.copyTo(ROI_2_1);
                part2_1.copyTo(ROI_1_1);

                // do azimuth align secondly
                int shift = int((vecAzimuth[i]) * resultImg1.cols / 360);
                cv::Mat resultImg2 = cv::Mat(resultImg1.rows, resultImg1.cols, CV_8UC3, cv::Scalar::all(0));
                cv::Mat part1 = resultImg1(cv::Rect(0, 0, resultImg1.cols - shift , resultImg1.rows));
                cv::Mat part2 = resultImg1(cv::Rect(resultImg1.cols - shift, 0, shift, resultImg1.rows));
                cv::Mat ROI_1 = resultImg2(cv::Rect(0, 0, shift, resultImg1.rows));
                cv::Mat ROI_2 = resultImg2(cv::Rect(shift, 0, resultImg1.cols - shift, resultImg1.rows));
                part1.copyTo(ROI_2);
                part2.copyTo(ROI_1);
                savedFileName = outputFolderName.toStdString() + "/" + ui->Area->text().toStdString() + "/grd/Aligned/" + fileNames[i].substr((panosFolderName.toStdString() + "/").length());
                std::cout << "Processing: " << savedFileName << std::endl;
                cv::imwrite(savedFileName, resultImg2);
            }
            else{
                // only do initial shift
                int initialshift = int(initialShift * srcImg.cols / 360);
                cv::Mat resultImg1 = cv::Mat(srcImg.rows, srcImg.cols, CV_8UC3, cv::Scalar::all(0));
                cv::Mat part1_1 = srcImg(cv::Rect(0, 0, srcImg.cols - initialshift , srcImg.rows));
                cv::Mat part2_1 = srcImg(cv::Rect(srcImg.cols - initialshift, 0, initialshift, srcImg.rows));
                cv::Mat ROI_1_1 = resultImg1(cv::Rect(0, 0, initialshift, srcImg.rows));
                cv::Mat ROI_2_1 = resultImg1(cv::Rect(initialshift, 0, srcImg.cols - initialshift, srcImg.rows));
                part1_1.copyTo(ROI_2_1);
                part2_1.copyTo(ROI_1_1);

                savedFileName = outputFolderName.toStdString() + "/" + ui->Area->text().toStdString() + "/grd/notAligned/" + fileNames[i].substr((panosFolderName.toStdString() + "/").length());
                std::cout << "Processing: " << savedFileName << std::endl;
                cv::imwrite(savedFileName, resultImg1);
            }
        }
        ui->geneGrd->setStyleSheet("background-color: rgb(0,255,0)");
    }
    else{
        std::cout << "No image under the path of original image!" << std::endl;
        ui->geneGrd->setStyleSheet("background-color: rgb(255,0,0)");
    }
    return;
}

void sketch::on_geneCSV_clicked()
{
    if(tifFileName.isEmpty() || vecLatLon.size() == 0
            || vecLatLonKML.size() == 0 || outputFolderName.isEmpty()){
        std::cout << "Lack of required files, please do check!" << std::endl;
        QMessageBox::warning(NULL, "Output configurations", "Lack of required files, please do check!", QMessageBox::Yes | QMessageBox::Yes);
        ui->geneCSV->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }

    QDir dir(outputFolderName + QString::fromStdString("/splits"));
    if(!dir.exists()){
        QDir().mkdir(outputFolderName + QString::fromStdString("/splits"));
    }

    vector<cv::String> filenamesGrd, filenamesSat;
    if(isNorthAlign){
        cv::glob((outputFolderName + "/" + ui->Area->text() + "/grd/Aligned/").toStdString(), filenamesGrd);
    }
    else{
        cv::glob((outputFolderName + "/" + ui->Area->text() + "/grd/notAligned/").toStdString(), filenamesGrd);
    }
    cv::glob((outputFolderName + "/" + ui->Area->text() + "/sat/" + ui->SatLevel->text() + "/").toStdString(), filenamesSat);

    if((filenamesGrd.size() != filenamesSat.size()) || (filenamesGrd.size() == 0) || (filenamesSat.size() == 0)){
        QMessageBox::warning(NULL, "Output configurations", "Empty folder or inquivalent file numbers!", QMessageBox::Yes | QMessageBox::Yes);
        ui->geneCSV->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }

    ofstream outFile;
    outFile.precision(8);
    if(isNorthAlign){
        outFile.open((outputFolderName + "/splits/" + ui->Area->text() + "_" + ui->SatLevel->text() + "_Aligned.csv").toStdString(), ios::out);

    }
    else{
        outFile.open((outputFolderName + "/splits/" + ui->Area->text() + "_" + ui->SatLevel->text() + "_notAligned.csv").toStdString(), ios::out);
    }
    cv::String savedNameGrd, savedNameSat;
    if(filenamesGrd.size() != 0)
    {
        coordinate LonLatLeftUpper = {vecLatLonKML.front().y, vecLatLonKML.front().x};
        coordinate LonLatRightBottom = {vecLatLonKML.back().y, vecLatLonKML.back().x};
        for(int i_img = 0, i = 0; i_img < filenamesGrd.size(); i_img++, i+=mImgInterval)
        {
            savedNameSat = filenamesSat[i_img].substr((outputFolderName).toStdString().length());
            savedNameGrd = filenamesGrd[i_img].substr((outputFolderName).toStdString().length());
            coordinate imgUV, UTMXY;
            int zone = LatLonToUTMXY(vecLatLon.at(i).x, vecLatLon.at(i).y, 50, UTMXY.x, UTMXY.y);
            if(isNorthAlign){
                outFile
                << savedNameSat << ','
                << savedNameGrd << ','
                << vecLatLon.at(i).x << ','
                << vecLatLon.at(i).y << ','
                << vecAzimuth[i] << ','
                << UTMXY.x << ','
                << UTMXY.y << ','
                << LatLonToImgUV(imgTif, LonLatLeftUpper, LonLatRightBottom, 50, 0, vecLatLon.at(i)).x << ','
                << LatLonToImgUV(imgTif, LonLatLeftUpper, LonLatRightBottom, 50, 0, vecLatLon.at(i)).y << ',' << endl;
            }
            else{
                outFile
                << savedNameSat << ','
                << savedNameGrd << ','
                << vecLatLon.at(i).x << ','
                << vecLatLon.at(i).y << ','
                << UTMXY.x << ','
                << UTMXY.y << ','
                << LatLonToImgUV(imgTif, LonLatLeftUpper, LonLatRightBottom, 50, 0, vecLatLon.at(i)).x << ','
                << LatLonToImgUV(imgTif, LonLatLeftUpper, LonLatRightBottom, 50, 0, vecLatLon.at(i)).y << ',' << endl;
            }
            cout << "wrtring into csv file: " << savedNameSat << "," << savedNameGrd << "," << endl;
        }
        outFile.close();
        ui->geneCSV->setStyleSheet("background-color: rgb(0,255,0)");
    }
    else
    {
        std::cout << "Incomplete images under the images path!" << std::endl;
        ui->geneCSV->setStyleSheet("background-color: rgb(255,0,0)");
        return;
    }
}

void sketch::on_ConfigConfirm_clicked()
{
    if((ui->SatLevel->text().toInt() > 17 && ui->SatLevel->text().toInt() < 21) &&
            (ui->SatWidth->text().toInt() > 0) &&
            (ui->ImgInterval->text().toInt() > 0)){

        QString content = "SatLevel ";
        content.append(ui->SatLevel->text());
        content.append(" SatWidth ");
        content.append(ui->SatWidth->text());
        content.append(" ImgInterval ");
        content.append(ui->ImgInterval->text());
        QMessageBox::information(NULL, "Output configurations", content, QMessageBox::Yes | QMessageBox::Yes);
        ui->ConfigConfirm->setStyleSheet("background-color: rgb(0,255,0)");

        mSatLevel = ui->SatLevel->text().toInt();
        mSatWidth = ui->SatWidth->text().toInt();
        mImgInterval = ui->ImgInterval->text().toInt();
        isNorthAlign = ui->NorthAlign->isChecked();
        return;
    }
    else{
        QMessageBox::warning(NULL, "Output configurations", "Invalide configuration", QMessageBox::Yes | QMessageBox::Yes);
        ui->ConfigConfirm->setStyleSheet("background-color: rgb(255,0,0)");

        return;
    }
    //cout << mSatLevel << " " << mSatWidth << " " << mImgInterval << " " << isNorthAlign << endl;
}

void sketch::on_loadAzi_clicked()
{
    if(!isNorthAlign)
    {
        QMessageBox::warning(NULL, "Error", "Pleace check 'NorthAlign' first", QMessageBox::Yes | QMessageBox::Yes);
        return;
    }
    AziFileName = QFileDialog::getOpenFileName(this, "Open Azimuth File", "", "*.txt");
    if(!AziFileName.isEmpty()){
        ui->loadAzi->setStyleSheet("background-color: rgb(0,255,0)");
        vecAzimuth = Azimuth2Vec(AziFileName.toStdString(), 5);
    }
    else{
        ui->loadAzi->setStyleSheet("background-color: rgb(255,0,0)");
    }
    qDebug("Azimuth file path = %s", qPrintable(AziFileName));
}

void sketch::on_shiftDisplay_textChanged(const QString &arg1)
{
    initialShift = ui->shiftDisplay->text().toFloat();

    if((initialShift > 180) || (initialShift < -180)){
        QMessageBox::warning(NULL, "Error", "Initial angle should between [-180, 180]!", QMessageBox::Yes | QMessageBox::Yes);
        return;
    }

    if(initialShift < 0){
        initialShift = 360 + initialShift;
    }

    std::vector<cv::String> fileNames;
    cv::glob(panosFolderName.toStdString() + "/", fileNames);
    cv::Mat imgPano = cv::imread(fileNames[1], 1);

    //image transition
    int shift = int(initialShift * imgPano.cols / 360);
    cv::Mat resultImg = cv::Mat(imgPano.rows, imgPano.cols, CV_8UC3, cv::Scalar::all(0));
    cv::Mat part1 = imgPano(cv::Rect(0, 0, imgPano.cols - shift , imgPano.rows));
    cv::Mat part2 = imgPano(cv::Rect(imgPano.cols - shift, 0, shift, imgPano.rows));
    cv::Mat ROI_1 = resultImg(cv::Rect(0, 0, shift, imgPano.rows));
    cv::Mat ROI_2 = resultImg(cv::Rect(shift, 0, imgPano.cols - shift, imgPano.rows));
    part1.copyTo(ROI_2);
    part2.copyTo(ROI_1);

    //颜色空间转换
    cv::Mat imgRGB = resultImg.clone();
    QImage Img;
    cv::cvtColor(resultImg, imgRGB, CV_BGR2RGB);
    Img = QImage((const uchar*)(imgRGB.data), imgRGB.cols, imgRGB.rows, imgRGB.cols * imgRGB.channels(), QImage::Format_RGB888);

    ui->labelPano->setScaledContents(true);
    ui->labelPano->setPixmap(QPixmap::fromImage(Img));

    return;
}
