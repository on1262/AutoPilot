#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutoPilot.h"
#include "Model.h"
#include "qtimer.h"
#include <QTimer>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "ViewCapture.h"
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
	//图像传输和图像识别
	ViewCapture* capture;
	void openCam();
	void imageCompareWithNode();
	//小车运动
	bool flagForward = false;
	bool flagBackward = false;
	bool flagLeft = false;
	bool flagRight = false;
	void carMoveForward();
	void carMoveBackward();
	void carTurnLeft();
	void carTurnRight();
	void carStop();
	void carAngleAdapt();
	void flushView();
	//蓝牙串口
	std::string textStr;
	void bufferUpdate();
	void sendContinueCmd();
	void sendDataToArduino();
	void bufferClear();
	//导航和轨迹绘制
	bool nowDrawingMode = false;
	void switchDrawingMode();
	void addNewNavigationPoint(); //设置导航点
	void startAutoNavigation(); //开启导航
	void saveMapToFile(); //保存当前地图到json中
	void loadMapFromFile();
private:
	Ui::MainWindow ui;
	QString mapFolderPath;
	/*map下就是一个地图*/
	void chooseMapFolder();
	void connectCarBluetoothSerial();
	QTimer* bufferUpdateTimer = nullptr;
};
