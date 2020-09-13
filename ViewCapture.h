#pragma once
#include <opencv2/opencv.hpp>
#include "qimage.h"
#include "qstring.h"
#include "qlabel.h"
class ViewCapture
{
public:
	static ViewCapture* _capture;
	static ViewCapture* getInstance();
	static ViewCapture* create(QLabel* labelWebCam);
	//Í¼Ïñ´«Êä
	QLabel* labelWebCam = nullptr;
	bool isCamConnected = false;
	cv::Mat nowDisplayingImg;
	cv::VideoCapture cap;
	QImage qt_image;
	std::string IPAddress;
	void openCam(QString cameraIP);
	void closeCam();
	void camUpdate();
	cv::Mat getNowFrame();
};

