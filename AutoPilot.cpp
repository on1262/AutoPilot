#include "AutoPilot.h"
#include "Model.h"
#include "qtimer.h"
#include "qpropertyanimation.h"
#include <iostream>
#include "qfiledialog.h"
#include "qscrollbar.h"
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
	connect(ui.ButtonChooseMapFolder, &QPushButton::clicked, this, &AutoPilot::chooseMapFolder);
	connect(ui.ButtonContinue, &QPushButton::clicked, this, &AutoPilot::sendContinueCmd);
	connect(ui.ButtonSwitchMode, &QPushButton::clicked, this, &AutoPilot::switchDrawingMode);
	connect(ui.ButtonStartNavigation, &QPushButton::clicked, this, &AutoPilot::startAutoNavigation);
	connect(ui.ButtonSetNavigationPoint, &QPushButton::clicked, this, &AutoPilot::addNewNavigationPoint);
	connect(ui.ButtonSendData, &QPushButton::clicked, this, &AutoPilot::sendDataToArduino);
	connect(ui.ButtonSaveMapToFile, &QPushButton::clicked, this, &AutoPilot::saveMapToFile);
	connect(ui.ButtonLoadMap, &QPushButton::clicked, this, &AutoPilot::loadMapFromFile);
	connect(ui.ButtonCompareWithNode, &QPushButton::clicked, this, &AutoPilot::imageCompareWithNode);
	connect(ui.ButtonClearBuffer, &QPushButton::clicked, this, &AutoPilot::bufferClear);
	//初始化debug输出
	Utils::setDebugBrowser(ui.BrowserDebug);
	//初始化图像传输
	connect(ui.ButtonStartVideoTransportation, &QPushButton::clicked, this, &AutoPilot::openCam);
	capture = ViewCapture::create(ui.labelWebCam);
	//初始化模型
	model = new autopilot::Model(ui.ViewNavigation, ui.LabelNavigationStatus,ui.BrowserBuffer, this);
	//初始化设置
	updateSettingsUI();
	//初始化小车控制模块的连接
	connect(ui.ButtonForward, &QPushButton::pressed, this, &AutoPilot::carMoveForward);
	connect(ui.ButtonForward, &QPushButton::released, this, &AutoPilot::carMoveForward);
	connect(ui.ButtonBackward, &QPushButton::pressed, this, &AutoPilot::carMoveBackward);
	connect(ui.ButtonBackward, &QPushButton::released, this, &AutoPilot::carMoveBackward);
	connect(ui.ButtonTurnLeft, &QPushButton::pressed, this, &AutoPilot::carTurnLeft);
	connect(ui.ButtonTurnLeft, &QPushButton::released, this, &AutoPilot::carTurnLeft);
	connect(ui.ButttonTurnRight, &QPushButton::pressed, this, &AutoPilot::carTurnRight);
	connect(ui.ButttonTurnRight, &QPushButton::released, this, &AutoPilot::carTurnRight);
	connect(ui.ButtonStop, &QPushButton::clicked, this, &AutoPilot::carStop);
	connect(ui.ButtonAdapt, &QPushButton::clicked, this, &AutoPilot::carAngleAdapt);
	//初始化view的刷新计时器
	model->flushTimer = new QTimer();
	model->flushTimer->setInterval(300);
	connect(model->flushTimer, &QTimer::timeout, this, &AutoPilot::flushView);
	model->flushTimer->start();
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
	mapFolderPath = Utils::mapFolderPath;
}

void AutoPilot::updateModelFromSettingsUI()
{
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
	Utils::mapFolderPath = mapFolderPath;
	model->isLoadTestMapWhenStart = ui.RadioLoadTestMapOnStart->isChecked();
}

void AutoPilot::updateCarControl()
{
	model->rotateAngle = ui.EditRotateAngle->text().toFloat();
	model->moveSpeed = ui.EditMoveSpeed->text().toFloat();
}

void AutoPilot::updateCV()
{
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

void AutoPilot::openCam()
{
	capture->openCam(ui.EditCameraIP->text());
}

void AutoPilot::imageCompareWithNode()
{
	model->imageCompareWithNode(ui.EditCompareNode->text().toInt());
}

void AutoPilot::carMoveForward()
{
	flagForward = !flagForward;

	model->carMoveForward(flagForward, "");
}

void AutoPilot::carMoveBackward()
{
	flagBackward = !flagBackward;

	model->carMoveBackward(flagBackward, "");
}

void AutoPilot::carTurnLeft()
{
	flagLeft = !flagLeft;
	model->carTurnLeft(flagLeft, "");
}

void AutoPilot::carTurnRight()
{
	flagRight = !flagRight;
	model->carTurnRight(flagRight, "");
}

void AutoPilot::carStop()
{
	if (model->isConnected() == true) {
		model->sendCmd2Arduino("S0000");
	}
}

void AutoPilot::carAngleAdapt()
{
	if (model->isConnected() == true) {
		model->sendCmd2Arduino("A0000");
	}

}

void AutoPilot::flushView()
{
	//刷新view内部内容
	model->flushView();
}

void AutoPilot::bufferUpdate()
{
	string str = model->listenOnce();
	if (str.empty() == true) return;
	model->readBuffer(QString::fromStdString(str));
}

void AutoPilot::sendContinueCmd()
{
	if (model->isConnected() == true) {
		model->serialWrite("E");
	}
}


void AutoPilot::sendDataToArduino()
{
	QString str = ui.EditSendData->text();
	model->serialWrite(str.toStdString());
}

void AutoPilot::bufferClear()
{
	textStr.clear();
	ui.BrowserBuffer->clear();
}

void AutoPilot::chooseMapFolder()
{
	mapFolderPath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose Map Folder"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "\\";
	Utils::log(false, "INFO: choosed directory:" + mapFolderPath.toStdString() + "\\");
}

void AutoPilot::connectCarBluetoothSerial()
{
	//尝试打开端口
	updateCarBlueTooth();
	if (model->connectBlueToothSerial() == true) {
		if (bufferUpdateTimer != nullptr) delete bufferUpdateTimer;
		bufferUpdateTimer = new QTimer(this);
		bufferUpdateTimer->setInterval(200);
		bufferUpdateTimer->start();
		textStr.clear();
		connect(bufferUpdateTimer, &QTimer::timeout, this, &AutoPilot::bufferUpdate);
	}
}



void AutoPilot::switchDrawingMode()
{
	if (nowDrawingMode == false) {
		nowDrawingMode = true;
		ui.ButtonSwitchMode->setText(Utils::toUnicode("取消绘制"));
	}
	else {
		nowDrawingMode = false;

		ui.ButtonSwitchMode->setText(Utils::toUnicode("开始绘制"));
		model->isNowDrawingPath = false;
		model->cancelNowPath();
	}
}

void AutoPilot::addNewNavigationPoint()
{
	model->addNavigationNode();
}

void AutoPilot::startAutoNavigation()
{
	if (ui.EditNavigationID->text().isEmpty() == false) {
		model->startAutoNavigation(ui.EditNavigationID->text().toInt());
	}
}

void AutoPilot::saveMapToFile()
{
	model->saveMapToFile();
}

void AutoPilot::loadMapFromFile()
{
	model->loadMap();
}