#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutoPilot.h"
#include "Model.h"
#include "qtimer.h"
#include <QTimer>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class AutoPilot : public QMainWindow
{
    Q_OBJECT

public:
	AutoPilot(QWidget *parent = Q_NULLPTR);
	autopilot::Model* model;
	//设置文件
	void updateSettingsUI(); //从model加载设定值
	void updateModelFromSettingsUI(); //从UI导入设定值到model
	void updateAutoPilot();
	void updateCarControl();
	void updateCV();
	void updateBlueToothSerial();
	void updateCarBlueTooth();
	//小车运动
	bool flagForward = false;
	bool flagBackward = false;
	bool flagLeft = false;
	bool flagRight = false;
	void carMoveForward();
	void carMoveBackward();
	void carTurnLeft();
	void carTurnRight();
	void flushView();
	//蓝牙串口
	std::string textStr;
	void bufferUpdate();
	void startControl();
	void sendDataToArduino();
	//图像传输
	QTimer *timer; //图像刷新计时器
	cv::VideoCapture cap;
	QImage qt_image;
	std::string IPAddress;
	void AutoPilot::openCam();
	void AutoPilot::closeCam();
	void AutoPilot::camUpdate();
	//导航和轨迹绘制
	bool nowDrawingMode = false;
	void switchDrawingMode();
	void addNewNavigationPoint(); //设置导航点
	void startAutoNavigation(); //开启导航
private:
	Ui::MainWindow ui;
	QString mapFolderPath;
	QString testFolderPath;
	QString cacheFolderPath;
	QString settingsFilePath;
	void chooseMapFolder();
	void chooseTestFolder();
	void chooseCacheFolder();
	void chooseSettingsFile();
	void connectCarBluetoothSerial();
	QTimer* bufferUpdateTimer = nullptr;
};
