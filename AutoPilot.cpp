#include "AutoPilot.h"
#include "Model.h"
#include "qtimer.h"
#include "qpropertyanimation.h"
#include <iostream>
#include "qfiledialog.h"
using namespace std;
using namespace cv;
AutoPilot::AutoPilot(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//初始化前端的逻辑连接
	connect(ui.ButtonFlushUI, &QPushButton::clicked, this, &AutoPilot::updateSettingsUI);
	connect(ui.ButtonFlushSettings, &QPushButton::clicked, this, &AutoPilot::updateModelFromSettingsUI);
	connect(ui.ButtonAutoPilotUpdate, &QPushButton::clicked, this, &AutoPilot::updateAutoPilot);
	connect(ui.ButtonCarControlUpDate, &QPushButton::clicked, this, &AutoPilot::updateCarControl);
	connect(ui.ButtonCVUpdate, &QPushButton::clicked, this, &AutoPilot::updateCV);
	connect(ui.ButtonAutoPilotUpdate, &QPushButton::clicked, this, &AutoPilot::updateBlueToothSerial);
	connect(ui.ButtonCarBlueToothConnect, &QPushButton::clicked, this, &AutoPilot::connectCarBluetoothSerial);
	connect(ui.ButtonConnectCar, &QPushButton::clicked, this, &AutoPilot::connectCarBluetoothSerial);
	connect(ui.ButtonChooseCacheFolder, &QPushButton::clicked, this, &AutoPilot::chooseCacheFolder);
	connect(ui.ButtonChooseDataPath, &QPushButton::clicked, this, &AutoPilot::chooseTestFolder);
	connect(ui.ButtonChooseMapFolder, &QPushButton::clicked, this, &AutoPilot::chooseMapFolder);
	connect(ui.ButtonChooseSettingsPath, &QPushButton::clicked, this, &AutoPilot::chooseSettingsFile);
	connect(ui.ButtonStartControl, &QPushButton::clicked, this, &AutoPilot::startControl);
	connect(ui.ButtonSwitchMode, &QPushButton::clicked, this, &AutoPilot::switchDrawingMode);
	connect(ui.ButtonStartNavigation, &QPushButton::clicked, this, &AutoPilot::startAutoNavigation);
	connect(ui.ButtonSetNavigationPoint, &QPushButton::clicked, this, &AutoPilot::addNewNavigationPoint);
	connect(ui.ButtonSendData, &QPushButton::clicked, this, &AutoPilot::sendDataToArduino);
	//初始化debug输出
	Utils::setDebugBrowser(ui.BrowserDebug);
	//初始化图像传输
	connect(ui.ButtonStartVideoTransportation, &QPushButton::clicked, this, &AutoPilot::openCam);
	timer = new QTimer(this);
	//初始化模型
	model = new autopilot::Model(ui.ViewNavigation, ui.LabelNavigationStatus, this);
	//初始化小车控制模块的连接
	connect(ui.ButtonForward, &QPushButton::pressed, this, &AutoPilot::carMoveForward);
	connect(ui.ButtonForward, &QPushButton::released, this, &AutoPilot::carMoveForward);
	connect(ui.ButtonBackward, &QPushButton::pressed, this, &AutoPilot::carMoveBackward);
	connect(ui.ButtonBackward, &QPushButton::released, this, &AutoPilot::carMoveBackward);
	connect(ui.ButtonTurnLeft, &QPushButton::pressed, this, &AutoPilot::carTurnLeft);
	connect(ui.ButtonTurnLeft, &QPushButton::released, this, &AutoPilot::carTurnLeft);
	connect(ui.ButttonTurnRight, &QPushButton::pressed, this, &AutoPilot::carTurnRight);
	connect(ui.ButttonTurnRight, &QPushButton::released, this, &AutoPilot::carTurnRight);
	//初始化view的刷新计时器
	model->flushTimer = new QTimer();
	model->flushTimer->setInterval(300);
	connect(model->flushTimer, &QTimer::timeout, this, &AutoPilot::flushView);
	//初始化目标点绘制
	ui.ButtonSetNavigationPoint->setCheckable(false);
}

void AutoPilot::updateSettingsUI()
{
	Utils::log(false, "INFO: updateSettingsUI");
	//更新设置界面所有的UI
	ui.EditCOM->setText(QString::number(model->portNum, 10));
	ui.EditBaudRate->setText(QString::number(model->baudRate, 10));
	ui.RadioConvertToBW->setChecked(model->isTranslateToBW);
	ui.EditBufferSize->setText(QString::number(model->bufferSize, 10));
	ui.EditBufferUpdateFrequency->setText(QString::number(model->bufferUpdateFrequency, 10));
	ui.EditCameraSamplingFrequency->setText(QString::number(model->cameraSamplingFrequency, 10));
	ui.EditCompressedWidth->setText(QString::number(model->compressedWidth, 10));
	ui.EditCompressedHeight->setText(QString::number(model->compressedHeight, 10));
	ui.EditMoveSpeed->setText(QString(to_string(model->moveSpeed).c_str()));
	ui.EditRotateAngle->setText(QString(to_string(model->rotateAngle).c_str()));
	ui.RadioLoadTestMapOnStart->setChecked(model->isLoadTestMapWhenStart);

	//更新路径
	mapFolderPath = model->mapFolderPath;
	testFolderPath = model->testFolder;
	cacheFolderPath = model->cacheFolder;
	settingsFilePath = model->settingPath;
}

void AutoPilot::updateModelFromSettingsUI()
{
	//更新setting位置
	model->setSettingPath(settingsFilePath);
	//各个模块全部更新
	updateAutoPilot();
	updateBlueToothSerial();
	updateCarBlueTooth();
	updateCarControl();
	updateCV();
	//写入json
	model->writeSettings();
}

void AutoPilot::updateAutoPilot()
{
	model->mapFolderPath = mapFolderPath;
	model->isLoadTestMapWhenStart = ui.RadioLoadTestMapOnStart->isChecked();
}

void AutoPilot::updateCarControl()
{
	model->rotateAngle = ui.EditRotateAngle->text().toFloat();
	model->moveSpeed = ui.EditMoveSpeed->text().toFloat();
}

void AutoPilot::updateCV()
{
	model->cacheFolder = cacheFolderPath;
	model->testFolder = testFolderPath;
	model->cameraSamplingFrequency = ui.EditCameraSamplingFrequency->text().toInt();
	model->compressedWidth = ui.EditCompressedWidth->text().toInt();
	model->compressedHeight = ui.EditCompressedHeight->text().toInt();
	model->isTranslateToBW = ui.RadioConvertToBW->isChecked();
}

void AutoPilot::updateBlueToothSerial()
{
	model->bufferSize = ui.EditBufferSize->text().toInt();
	model->bufferUpdateFrequency = ui.EditBufferUpdateFrequency->text().toInt();
}

void AutoPilot::updateCarBlueTooth()
{
	model->portNum = ui.EditCOM->text().toInt();
	model->baudRate = ui.EditBaudRate->text().toInt();
}

void AutoPilot::carMoveForward()
{
	flagForward = !flagForward;

	model->carMoveForward(flagForward);
}

void AutoPilot::carMoveBackward()
{
	flagBackward = !flagBackward;

	model->carMoveBackward(flagBackward);
}

void AutoPilot::carTurnLeft()
{
	flagLeft = !flagLeft;
	model->carTurnLeft(flagLeft);
}

void AutoPilot::carTurnRight()
{
	flagRight = !flagRight;
	model->carTurnRight(flagRight);
}

void AutoPilot::flushView()
{
	model->flushView(); 
}

void AutoPilot::serialTextUpdate()
{
	string str = model->listenOnce();
	if (str.empty() == true) return;
	textStr += str; //这里如果直接用browser的append,会出现奇怪的数据分割现象。
	ui.BrowserBuffer->setText(QString::fromStdString(textStr));
}

void AutoPilot::startControl()
{
	isControllingStart = true;
	textStr.clear();
	ui.BrowserBuffer->clear();
	model->CmdsCount = 0; //重置指令数量
}

void AutoPilot::sendDataToArduino()
{
	QString str = ui.EditSendData->text();
	model->serialWrite(str.toStdString());
}

void AutoPilot::chooseMapFolder()
{
	mapFolderPath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose Map Folder"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	 Utils::log(false,"INFO: choosed directory:" + mapFolderPath.toStdString());
}

void AutoPilot::chooseTestFolder()
{
	testFolderPath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose Test Folder"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	Utils::log(false, "INFO: choosed directory:" + testFolderPath.toStdString());
}

void AutoPilot::chooseCacheFolder()
{
	cacheFolderPath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose Cache Folder"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	Utils::log(false, "INFO: choosed directory:" + cacheFolderPath.toStdString());
}

void AutoPilot::chooseSettingsFile()
{
	settingsFilePath = QFileDialog::getOpenFileUrl(
		this,
		tr("Choose settings.json"),
		QUrl("/home"),
		"Settings (*.json)").toString();
	Utils::log(false, "INFO: choosed file:" + settingsFilePath.toStdString());
}

void AutoPilot::connectCarBluetoothSerial()
{
	//尝试打开端口
	updateCarBlueTooth();
	if (model->connectBlueToothSerial() == true) {
		if (bufferUpdateTimer != nullptr) delete bufferUpdateTimer;
		bufferUpdateTimer = new QTimer(this);
		bufferUpdateTimer->setInterval(50);
		bufferUpdateTimer->start();
		textStr.clear();
		connect(bufferUpdateTimer, &QTimer::timeout, this, &AutoPilot::serialTextUpdate);
	}
}

void AutoPilot::openCam()
{
	IPAddress = ("http://" + ui.EditCameraIP->text() + "/capture").toStdString();
	cap.open(IPAddress);
	if (!cap.isOpened())  // Check if we succeeded
	{
		Utils::log(false, "Camera is not open");
	}
	else
	{
		Utils::log(false, "Camera is open");
		connect(timer, &QTimer::timeout, this, &AutoPilot::camUpdate);
		model->isCamConnected = true;
		timer->start(50);
	}
}

void AutoPilot::closeCam()
{
	disconnect(timer, &QTimer::timeout, this, &AutoPilot::camUpdate);
	cap.release();
	Mat image = Mat::zeros(model->nowDisplayingImg.size(), CV_8UC3);
	qt_image = QImage((const unsigned char*)(image.data), image.cols, image.rows, QImage::Format_RGB888);
	ui.labelWebCam->setPixmap(QPixmap::fromImage(qt_image));
	ui.labelWebCam->resize(ui.labelWebCam->pixmap()->size());
	model->isCamConnected = false;
	Utils::log(false, "INFO: camera closed.");
}

void AutoPilot::camUpdate()
{
	if (!cap.isOpened())
	{
		closeCam();
		return;
	}
	cap.open(IPAddress);
	cap >> model->nowDisplayingImg;
	cvtColor(model->nowDisplayingImg, model->nowDisplayingImg, COLOR_BGR2RGB);
	qt_image = QImage((const unsigned char*)(model->nowDisplayingImg.data), model->nowDisplayingImg.cols, model->nowDisplayingImg.rows, QImage::Format_RGB888);
	ui.labelWebCam->setPixmap(QPixmap::fromImage(qt_image));
	ui.labelWebCam->resize(ui.labelWebCam->pixmap()->size());
}

void AutoPilot::switchDrawingMode()
{
	if (nowDrawingMode == false) {
		nowDrawingMode = true;
		ui.ButtonSwitchMode->setText(QString("取消绘制"));
		ui.ButtonSetNavigationPoint->setCheckable(true);
	}
	else {
		nowDrawingMode = false;
		ui.ButtonSwitchMode->setText(QString("开始绘制"));
		ui.ButtonSetNavigationPoint->setCheckable(false);
	}
}

void AutoPilot::addNewNavigationPoint()
{
}

void AutoPilot::startAutoNavigation()
{
}