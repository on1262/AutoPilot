#include "Model.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "qjsondocument.h"
#include "qgraphicsitemanimation.h"
#include "qfile.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qdebug.h"
#include "Utils.h"

using namespace std;
using namespace cv;
using namespace autopilot;

viewVector autopilot::Model::SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath)
{
	cv::Mat img_1 = cv::imread(leftFilePath);
	cv::Mat img_2 = cv::imread(rightFilePath);

	if (!img_1.data || !img_2.data)
	{
		std::cout << " --(!) Error reading images " << std::endl;
	}
	//-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
	int minHessian = 400;
	Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create();
	detector->setHessianThreshold(minHessian);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	detector->detectAndCompute(img_1, Mat(), keypoints_1, descriptors_1);
	detector->detectAndCompute(img_2, Mat(), keypoints_2, descriptors_2);
	//-- Step 2: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);
	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= min(2 * min_dist, (double)matchThreshold))
		{
			good_matches.push_back(matches[i]);
		}
	}
	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//计算方向
	size_t matchesSize = good_matches.size();
	viewVector vec{ 0,0,0 };
	for (auto i = good_matches.begin(); i != good_matches.end(); i++) {
		//归一化并转换到中心坐标系, x大于0代表R相对于L相机向右移动，同理y是向上移动
		//center为正代表R相对于L放大，反之是缩小
		//相机移动方向和物体移动方向相反
		cv::Point2f p1{ keypoints_1[i->queryIdx].pt.x / img_1.cols - 0.5f, keypoints_1[i->queryIdx].pt.y / img_1.rows - 0.5f };
		cv::Point2f p2{ keypoints_2[i->trainIdx].pt.x / img_2.cols - 0.5f, keypoints_2[i->trainIdx].pt.y / img_2.rows - 0.5f };
		vec.x += p1.x - p2.x;
		vec.y += p2.y - p1.y;
		vec.center += ((p2 - p1).dot(p1) / sqrtf(p1.x * p1.x + p1.y * p1.y));
	}
	vec.x /= matchesSize;
	vec.y /= matchesSize;
	vec.center /= matchesSize;
	cout << "normalized delta x=" << vec.x << endl;
	cout << "normalized delta y=" << vec.y << endl;
	cout << "normalized center=" << vec.center << endl;
	//-- Show detected matches
	imshow(leftFilePath + " to " + rightFilePath, img_matches);
	return vec;
}

void autopilot::Model::SURFMutiFiles(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles)
{
	if (leftFiles.size() != rightFiles.size()) {
		cout << "ERROR: SURFTest(): Unmatched size between leftFiles and rightFiles.";
		return;
	}
	for (int i = 0; i != leftFiles.size(); i++) {
		cout << "INFO: SURF({" + std::to_string(matchThreshold) + "," + leftFiles[i] + "," + rightFiles[i] + ")" << endl;
		SURF(matchThreshold, leftFiles[i], rightFiles[i]);
	}
	cout << "SURF test end." << endl;
	return;
}

void autopilot::Model::SURFTest()
{
	std::vector<std::string> leftFiles;
	std::vector<std::string> rightFiles;
	std::string testImgPath = Utils::getDataFolder().toStdString() + "test//";
	leftFiles.push_back(testImgPath + "test3_origin.jpg");
	rightFiles.push_back(testImgPath + "test3_scaleup.jpg");
	SURFMutiFiles(0.12, rightFiles, leftFiles);
}

void autopilot::Model::IPCameraTest()
{
	cv::VideoCapture vcap;
	cv::Mat image;
	cout << "IPCamera Test" << endl;
	// This works on a D-Link CDS-932L
	const std::string videoStreamAddress = "http://admin:123456789@192.168.88.1:81/video?x.mjpeg";

	//open the video stream and make sure it's opened
	if (!vcap.open(videoStreamAddress)) {
		std::cout << "Error opening video stream or file" << std::endl;
	}

	for (;;) {
		if (!vcap.read(image)) {
			std::cout << "No frame" << std::endl;
			cv::waitKey();
		}
		cv::imshow("Output Window", image);
		if (cv::waitKey(1) >= 0) break;
	}
}

void autopilot::Model::readSettings()
{
	//读取json文件
	QString val;
	QFile file;
	file.setFileName(QString(settingPath));
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	val = file.readAll();
	file.close();
	try {
		QJsonObject settings = QJsonDocument::fromJson(val.toUtf8()).object();
		QJsonObject carControl = settings.value(QString("CarControl")).toObject();
		rotateAngle = carControl["RotateAngle"].toDouble();
		moveSpeed = carControl["MoveSpeed"].toDouble();
		QJsonObject autoPilot = settings.value(QString("AutoPilot")).toObject();
		mapFolderPath = autoPilot["MapFolderPath"].toString();
		isLoadTestMapWhenStart = autoPilot["LoadTestMapWhenStart"].toBool();
		QJsonObject carBlueTooth = settings.value(QString("CarBlueTooth")).toObject();
		portNum = carBlueTooth["COM"].toInt();
		baudRate = carBlueTooth["BaudRate"].toInt();
		QJsonObject blueToothSerial = settings.value(QString("BlueToothSerial")).toObject();
		bufferSize = blueToothSerial["BufferSize"].toInt();
		bufferUpdateFrequency = blueToothSerial["BufferUpdateFrequency"].toInt();
		QJsonObject cv = settings.value(QString("CV")).toObject();
		testFolder = cv["TestFolder"].toString();
		cacheFolder = cv["CacheFolder"].toString();
		cameraSamplingFrequency = cv["CameraSamplingFrequency"].toInt();
		compressedWidth = cv["CompressedWidth"].toInt();
		compressedHeight = cv["CompressedHeight"].toInt();
		isTranslateToBW = cv["TranslateToBW"].toBool();
	}
	catch (exception e) {
		cout << "ERROR: Failed to load setting file." << endl;
		cout << e.what() << endl;
	}
}

void autopilot::Model::writeSettings()
{
	QFile file(settingPath);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QJsonParseError JsonParseError;
	QJsonDocument JsonDocument = QJsonDocument::fromJson(file.readAll(), &JsonParseError);
	file.close();
	QJsonObject RootObject = JsonDocument.object();
	//修改json文件
	QJsonValueRef carControl = RootObject.find("CarControl").value();
	QJsonObject m_addvalue = carControl.toObject();
	m_addvalue.insert("RotateAngle", rotateAngle);
	m_addvalue.insert("MoveSpeed", moveSpeed);
	carControl = m_addvalue;

	QJsonValueRef AutoPilot = RootObject.find("AutoPilot").value();
	m_addvalue = AutoPilot.toObject();
	m_addvalue.insert("MapFolderPath", mapFolderPath);
	m_addvalue.insert("LoadTestMapWhenStart", isLoadTestMapWhenStart);
	AutoPilot = m_addvalue;

	QJsonValueRef CarBlueTooth = RootObject.find("CarBlueTooth").value();
	m_addvalue = CarBlueTooth.toObject();
	m_addvalue.insert("COM", portNum);
	m_addvalue.insert("BaudRate", baudRate);
	CarBlueTooth = m_addvalue;

	QJsonValueRef BlueToothSerial = RootObject.find("BlueToothSerial").value();
	m_addvalue = BlueToothSerial.toObject();
	m_addvalue.insert("BufferSize", bufferSize);
	m_addvalue.insert("BufferUpdateFrequency", bufferUpdateFrequency);
	BlueToothSerial = m_addvalue;

	QJsonValueRef CV = RootObject.find("CV").value();
	m_addvalue = CV.toObject();
	m_addvalue.insert("TestFolder", testFolder);
	m_addvalue.insert("CacheFolder", cacheFolder);
	m_addvalue.insert("CameraSamplingFrequency", cameraSamplingFrequency);
	m_addvalue.insert("CompressedWidth", compressedWidth);
	m_addvalue.insert("CompressedHeight", compressedHeight);
	m_addvalue.insert("TranslateToBW", isTranslateToBW);
	CV = m_addvalue;
	//保存json文件
	JsonDocument.setObject(RootObject); // set to json document
	file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
	file.write(JsonDocument.toJson());
	file.close();
}

void autopilot::Model::setSettingPath(QString settingPath)
{
	this->settingPath = settingPath;
}

void autopilot::Model::flushCarViewPosition()
{
	ViewPoint rotation;
	rotation.setRotationDeg(state.direction);
	ViewPoint realPos = state.pos;
	realPos.x += state.nowLength * rotation.x;
	realPos.y += state.nowLength * rotation.y;
	QPoint p = QPoint(
		realPos.x * real2ViewCoef + pView->size().width / 2.0f,
		-realPos.y * real2ViewCoef + pView->size().height / 2.0f
	);
	car->setPos(p);
	return;
}

void autopilot::Model::flushCarViewRotation()
{
	car->setRotation(state.direction + state.nowRotation);
	return;
}




void autopilot::Model::ViewInit(QWidget* window)
{
	auto scene = new QGraphicsScene(window);
	pView->setScene(scene);
	//添加背景
	auto bg = new QGraphicsPixmapItem();
	bg->setPixmap(Utils::getUIFolder() + "background.png");
	bg->setPos(QPoint(0, 0));
	scene->addItem(bg);
	//添加小车
	car = new ViewItemCar();

	car->init(QPoint((double)scene->width() / 2.0, (double)scene->height() / 2.0));
	scene->addItem(car);
}

void autopilot::Model::addViewImage()
{
}

autopilot::Model::Model(QGraphicsView* view, QWidget* window)
{
	this->pView = view;
	setSettingPath(Utils::getSettingsFolder() + "settings.json");
	readSettings();
	ViewInit(window);
	//测试
	//SURFTest();
}

void autopilot::Model::carMoveForward(bool flag)
{
	Utils::log(false, std::string("Car move forward:") + (flag ? "true" : "false"));
	if (isConnected() == true) {
		if (flag == true) {
			state.nowLength = 0;
			state.exceptedLength = 200;
			state.s = carState::forward;
			serialWrite(QString("F0200"));
		}
		else {
			serialWrite(QString("S0000"));
		}
	}

}

void autopilot::Model::carMoveBackward(bool flag)
{
	Utils::log(false, std::string("Car move forward:") + (flag ? "true" : "false"));
	if (isConnected() == true) {
		if (flag == true) {
			state.nowLength = 0;
			state.exceptedLength = -200;
			state.s = carState::backward;
			serialWrite(QString("B0200"));
		}
		else {
			serialWrite(QString("S0000"));
		}
	}
}

void autopilot::Model::carTurnLeft(bool flag)
{
	Utils::log(false, std::string("Car turn left:") + (flag ? "true" : "false"));
	if (isConnected() == true) {
		if (flag == true) {
			state.nowRotation = 0;
			state.exceptedRotation = 360;
			state.s = carState::turnLeft;
			serialWrite(QString("L0360"));
		}
		else {
			serialWrite(QString("S0000"));
		}
	}
}

void autopilot::Model::carTurnRight(bool flag)
{
	Utils::log(false, std::string("Car turn right:") + (flag ? "true" : "false"));
	if (isConnected() == true) {
		if (flag == true) {
			state.nowRotation = 0;
			state.exceptedRotation = -360;
			state.s = carState::turnLeft;
			serialWrite(QString("L0360"));
		}
		else {
			serialWrite(QString("S0000"));
		}
	}
}

void autopilot::Model::setFlushTimer()
{
	if (flushTimer != nullptr) flushTimer->start();
}

void autopilot::Model::flushView()
{
	flushCarViewRotation();
	flushCarViewPosition();
	switch (state.s) {
	case carState::stopped:
		//更新角度
		state.direction += state.nowRotation;
		if (state.direction > 360.0f) state.direction -= 360.0f;
		if (state.direction < -360.0f) state.direction += 360.0f;
		state.nowRotation = 0;
		state.exceptedRotation = 0;
		//更新距离
		ViewPoint rotation;
		rotation.setRotationDeg(state.direction);
		state.pos.x += state.nowLength * rotation.x;
		state.pos.y += state.nowLength * rotation.y;
		//暂停更新
		flushTimer->stop();
		break;
	case carState::shutdown:
		Utils::log(false, "flushView: car shutdown.");
		break;
	}
}

void autopilot::Model::setNavigationNode()
{
	if (isNowConfiguringPath == true) {
		//如果正在配置目标点,则结束这段配置
		nowPath->pathEnd();
		paths.push_back(nowPath);
	}
	else {
		//如果没有配置目标点，则新增目标点
		ViewPoint direct;
		direct.setRotationDeg(state.direction);
		nowPath = new ViewPath(state.pos, direct);
	}
}

bool autopilot::Model::connectBlueToothSerial()
{
	if (arduino != nullptr) delete arduino;
	arduino = new SerialPort(getPortName().c_str());
	if (arduino->isConnected() == false) {
		cout << "ERROR: Failed to connect Serial:" << getPortName().c_str() << endl;
	}
	else {
		cout << "INFO: serial port connected. COM=" << portNum << endl;
		return true;
	}
	return arduino->isConnected();
}

bool autopilot::Model::getCarSerialStatus()
{
	return isCarSerialPortActivated;
}

std::string autopilot::Model::getPortName()
{
	return "\\\\.\\COM" + to_string(portNum);
}

void autopilot::Model::setPortName(int num)
{
	portNum = num;
}

std::string autopilot::Model::listenOnce()
{
	if (arduino == nullptr) return string();
	if (arduino->isConnected() == true && arduino->readSerialPort(incomingData, MAX_DATA_LENGTH - 1) > 0) {
		return string(incomingData);
	}
	return string();
}

void autopilot::Model::readBuffer(QString str) //对信息进行预处理
{
	str.remove("\n");
	if (str.isEmpty() == false) {
		cmdStr.append(str);
		//四种情况：-XXX; -XXX;-X -XXX;- -XXX-XX;
		QStringList data = cmdStr.split(QChar('-'), Qt::SkipEmptyParts);
		if (data[data.size() - 1].endsWith(';') == false) {
			data.pop_back(); //如果最后一个指令接收不全，则剔除
		}
		if (data.size() == CmdsCount) {
			return; //如果剔除后没有接到新指令，则不执行操作
		}
		while (CmdsCount < data.size()) {
			CmdsCount++;
			//读取新指令
			data[CmdsCount].remove(";");
			readArduinoCmd(data[CmdsCount]);
		}
	}
}

void autopilot::Model::serialWrite(QString str)
{
	if (arduino == nullptr) return;
	else {
		bool result = arduino->writeSerialPort(str.toStdString().c_str(), MAX_DATA_LENGTH);
		if (result == false) {
			Utils::log(true, "serialWrite: failed to write str.");
		}
	}
}

void autopilot::Model::readArduinoCmd(QString str)
{
	std::string s = str.toStdString();
	switch (str[0].toLatin1())
	{
	case 'E': 
		state.s = carState::stopped;
		break;
	case 'F':
		//根据当前车状态判断如何更新数据
		if (state.s == carState::forward) {
			state.nowLength = Utils::convert<int>(s.substr(1, s.size()));
		}
		else if (state.s == carState::forward) {
			state.nowLength = -1 * Utils::convert<int>(s.substr(1, s.size()));
		}
		break;
	case 'R':
		if (state.s == carState::turnLeft) {
			state.nowRotation = Utils::convert<int>(s.substr(1, s.size()));
		}
		else if (state.s == carState::forward) {
			state.nowRotation = -1 * Utils::convert<int>(s.substr(1, s.size()));
		}
		break;
	case 'D':
		break; //debug信息已经输出在屏幕上了，不需要处理
	case 'S':
		state.s = carState::shutdown;
		break;
	default:
		Utils::log(true, "read arduino cmd: illegal command syntax");
		break;
	}
}

bool autopilot::Model::isConnected()
{
	if (arduino == nullptr) return false;
	else return arduino->isConnected();
}