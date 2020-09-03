#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutoPilot.h"
#include "Model.h"

class AutoPilot : public QMainWindow
{
    Q_OBJECT

public:
	AutoPilot(QWidget *parent = Q_NULLPTR);
	autopilot::Model* model;
	void updateSettingsUI(); //从model加载设定值
	void updateModelFromSettingsUI(); //从UI导入设定值到model
	void updateAutoPilot();
	void updateCarControl();
	void updateCV();
	void updateBlueToothSerial();
	void updateCarBlueTooth();
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
	void bufferTextUpdate();
	QTimer* bufferUpdateTimer = nullptr;
};
