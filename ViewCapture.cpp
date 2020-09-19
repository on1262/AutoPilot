#include "ViewCapture.h"
#include "Utils.h"
#include "qlabel.h"
using namespace cv;

ViewCapture* ViewCapture::_capture = nullptr;
ViewCapture * ViewCapture::getInstance()
{
	return _capture;
}

ViewCapture * ViewCapture::create(QLabel * labelWebCam)
{
	_capture = new ViewCapture();
	_capture->labelWebCam = labelWebCam;
	return getInstance();
}

void ViewCapture::openCam(QString cameraIP)
{
	IPAddress = ("http://" + cameraIP + "/capture").toStdString();
	cap.open(IPAddress);
	if (!cap.isOpened())  // Check if we succeeded
	{
		Utils::log(false, "Camera is not open");
	}
	else
	{
		Utils::log(false, "Camera is open");
		isCamConnected = true;
	}
}

void ViewCapture::closeCam()
{
	cap.release();
	Mat image = Mat::zeros(nowDisplayingImg.size(), CV_8UC3);
	qt_image = QImage((const unsigned char*)(image.data), image.cols, image.rows, QImage::Format_RGB888);
	labelWebCam->setPixmap(QPixmap::fromImage(qt_image));
	labelWebCam->resize(labelWebCam->pixmap()->size());
	isCamConnected = false;
	Utils::log(false, "INFO: camera closed.");
}

void ViewCapture::camUpdate()
{
	if (!cap.isOpened())
	{
		closeCam();
		return;
	}
	cap.open(IPAddress);
	cap >> nowDisplayingImg;
	if (nowDisplayingImg.empty() == false) {
		cvtColor(nowDisplayingImg, nowDisplayingImg, COLOR_BGR2RGB);
		qt_image = QImage((const unsigned char*)(nowDisplayingImg.data), nowDisplayingImg.cols, nowDisplayingImg.rows, QImage::Format_RGB888);
		labelWebCam->setPixmap(QPixmap::fromImage(qt_image));
		labelWebCam->resize(labelWebCam->pixmap()->size());
	}
}

cv::Mat ViewCapture::getNowFrame()
{
	camUpdate();
	return nowDisplayingImg.clone(); //如果不克隆，生成的所有图片都是一个引用
}
