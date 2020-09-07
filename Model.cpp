#include "Model.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "qjsondocument.h"
#include "qgraphicsitemanimation.h"
#include "qgraphicsitem.h"
#include "qfile.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qdebug.h"
#include "qstring.h"
#include "Utils.h"

using namespace std;
using namespace cv;
using namespace autopilot;

viewVector autopilot::Model::SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath)//model.cpp只改了这个函数，但精度不如用robustMatcher
{
	std::vector<cv::DMatch> matches;
	std::vector<cv::KeyPoint> keypoints1;
	std::vector<cv::KeyPoint> keypoints2;
	viewVector vec1 = b.match(leftFilePath, rightFilePath, matches, keypoints1, keypoints2);
	if (!(vec1.x == -255 && vec1.y == -255 && vec1.center == -255)) {
		return vec1;
	}
	viewVector vec_final{ 0,0,0 };
	int minHessian = 400; bool match_judge = false;
	do
	{
		cv::Mat image1, image2;
		cv::Mat img1 = cv::imread(leftFilePath);
		cv::Mat img2 = cv::imread(rightFilePath);
		resize(img1, image1, Size(507, 380));
		resize(img2, image2, Size(507, 380));
		if (!image1.data || !image2.data)
		{
			Utils::log(false, " --(!) Error reading images ");
		}
		//-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
		Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create();
		detector->setHessianThreshold(minHessian);
		std::vector<KeyPoint> keypoints1, keypoints2;
		Mat descriptors_1, descriptors_2;
		detector->detectAndCompute(image1, Mat(), keypoints1, descriptors_1);
		detector->detectAndCompute(image2, Mat(), keypoints2, descriptors_2);
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
		//计算方向
		size_t matchesSize = good_matches.size();
		viewVector vec{ 0,0,0 };
		for (auto i = good_matches.begin(); i != good_matches.end(); i++) {
			//归一化并转换到中心坐标系, x大于0代表R相对于L相机向右移动，同理y是向上移动
			//center为正代表R相对于L放大，反之是缩小
			//相机移动方向和物体移动方向相反
			cv::Point2f p1{ keypoints1[i->queryIdx].pt.x / image1.cols - 0.5f, keypoints1[i->queryIdx].pt.y / image1.rows - 0.5f };
			cv::Point2f p2{ keypoints2[i->trainIdx].pt.x / image2.cols - 0.5f, keypoints2[i->trainIdx].pt.y / image2.rows - 0.5f };
			vec.x += p1.x - p2.x;
			vec.y += p2.y - p1.y;
			vec.center += ((p2 - p1).dot(p1) / sqrtf(p1.x * p1.x + p1.y * p1.y));
		}
		vec.x /= matchesSize;
		vec.y /= matchesSize;
		vec.center /= matchesSize;
		std::vector<KeyPoint> keypoints_final1, keypoints_final2;  

		for (auto i = good_matches.begin(); i != good_matches.end(); i++) {
			//归一化并转换到中心坐标系, x大于0代表R相对于L相机向右移动，同理y是向上移动
			//center为正代表R相对于L放大，反之是缩小
			//相机移动方向和物体移动方向相反
			cv::Point2f p1{ keypoints1[i->queryIdx].pt.x / image1.cols - 0.5f, keypoints1[i->queryIdx].pt.y / image1.rows - 0.5f };
			cv::Point2f p2{ keypoints2[i->trainIdx].pt.x / image2.cols - 0.5f, keypoints2[i->trainIdx].pt.y / image2.rows - 0.5f };
			if ((p1.x - p2.x)*vec.x > 0 && (p2.y - p1.y)*vec.y > 0)//同向
			{
				keypoints_final1.push_back(keypoints1[i->queryIdx]);
				keypoints_final2.push_back(keypoints2[i->trainIdx]);
			}
		}
		Mat descriptors_final1, descriptors_final2;
		detector->compute(image1, keypoints_final1, descriptors_final1);
		detector->compute(image2, keypoints_final2, descriptors_final2);
		//-- Step 2: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher_final; std::vector< DMatch > matches_final;
		matcher_final.match(descriptors_final1, descriptors_final2, matches_final);
		double max_dist_final = 0; double min_dist_final = 100;
		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < descriptors_final1.rows; i++)
		{
			double dist = matches_final[i].distance;
			if (dist < min_dist_final) min_dist_final = dist;
			if (dist > max_dist_final) max_dist_final = dist;
		}
		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
		//-- small)
		//-- PS.- radiusMatch can also be used here.

		for (int i = 0; i < descriptors_final1.rows; i++)
		{
			if (matches_final[i].distance <= min(2 * min_dist_final, (double)matchThreshold))
			{
				matches_final.push_back(matches_final[i]);
			}
		}
		size_t matchesSize_final = matches_final.size();
		//计算方向
		if (matches_final.size() != 0)
		{
			for (auto i = matches_final.begin(); i != matches_final.end(); i++) {
				//归一化并转换到中心坐标系, x大于0代表R相对于L相机向右移动，同理y是向上移动
				//center为正代表R相对于L放大，反之是缩小
				//相机移动方向和物体移动方向相反
				cv::Point2f p1{ keypoints_final1[i->queryIdx].pt.x / image1.cols - 0.5f, keypoints_final1[i->queryIdx].pt.y / image1.rows - 0.5f };
				cv::Point2f p2{ keypoints_final2[i->trainIdx].pt.x / image2.cols - 0.5f, keypoints_final2[i->trainIdx].pt.y / image2.rows - 0.5f };
				vec_final.x += p1.x - p2.x;
				vec_final.y += p2.y - p1.y;
				vec_final.center += ((p2 - p1).dot(p1) / sqrtf(p1.x * p1.x + p1.y * p1.y));
				/*vec.center += ((p2 - p1).dot(p1) / sqrtf(p1.x * p1.x + p1.y * p1.y));*/
			}
			vec_final.x /= matchesSize_final;
			vec_final.y /= matchesSize_final;
			vec_final.center = vec.center;
			Utils::log(false, "normalized delta x=" + std::to_string(vec_final.x));
			Utils::log(false, "normalized delta y=" + std::to_string(vec_final.y));
			Utils::log(false, "normalized center(surf)=" + std::to_string( vec_final.center));

			//-- Draw only "good" matches
			Mat img_matches;
			drawMatches(image1, keypoints_final1, image2, keypoints_final2,
				matches_final, img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

			//-- Show detected matches
			Mat dst; double scale = 0.5;
			resize(img_matches, dst, Size(img_matches.cols*scale, img_matches.rows*scale));
			imshow(leftFilePath + " to " + rightFilePath, dst);
			return vec_final;
		}
		else
		{
			minHessian -= 25;
			if (minHessian <= 0)
			{
				return vec_final;
			}
		}
	} while (match_judge == false);
	return vec_final;
}

void autopilot::Model::SURFMutiFiles(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles)
{
	if (leftFiles.size() != rightFiles.size()) {
		Utils::log(true, " SURFTest(): Unmatched size between leftFiles and rightFiles.");
		return;
	}
	for (int i = 0; i != leftFiles.size(); i++) {
		Utils::log(false, "SURF({" + std::to_string(matchThreshold) + "," + leftFiles[i] + "," + rightFiles[i] + ")");
		SURF(matchThreshold, leftFiles[i], rightFiles[i]);
	}
	Utils::log(false, "SURF test end.");
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
		Utils::log(true, "ERROR: Failed to load setting file.");
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

void autopilot::Model::cmdFinished(bool isStoppedByError)
{
	//清空状态，并更新小车位置
	if (state.s == carState::forward || state.s == carState::backward) {
		flushCarViewPosition(true);
	}
	if (state.s == carState::turnLeft || state.s == carState::turnRight) {
		flushCarViewRotation(true);
	}
	state.s = carState::stopped;
}

void autopilot::Model::flushCarViewPosition(bool isFlushPos)
{
	if (isNowDrawingPath == true) {
		//在绘制路线时，如果小车前后移动了：
		if (nowPath->nowStep->stepStatus == ViewPathStep::rotating) {
			//如果处于第一阶段，则强行进入第二阶段
			nowPath->nowStep->stepStatus = ViewPathStep::expanding;
		}	
	}
	ViewPoint rotation;
	rotation.setRotationDeg(state.direction);
	ViewPoint realPos = state.pos;
	realPos.x += state.nowLength * rotation.x;
	realPos.y += state.nowLength * rotation.y;
	QPoint p = QPoint(
		realPos.x * real2ViewCoef + pView->size().width() / 2.0f,
		-realPos.y * real2ViewCoef + pView->size().height() / 2.0f
	);
	car->setPos(p);
	if (isFlushPos == true) {
		state.pos = realPos; //将pos刷新
	}

	return;
}

void autopilot::Model::flushCarViewRotation(bool isFlushDirection)
{
	if (isNowDrawingPath == true) {
		//在绘制路线时，如果小车旋转了：
		if (nowPath->nowStep->stepStatus == ViewPathStep::expanding) {
			//如果处于第二阶段，则强行进入下一节点
			nowPath->addStep();
		}
	}
	car->setRotation(state.direction + state.nowRotation);
	if (isFlushDirection == true) {
		state.direction += state.nowRotation;
	}
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

ViewImage* autopilot::Model::addViewImageFromNowPos()
{
	if (isCamConnected == true) {
		ViewImage* img = ViewImage::createFromMat(Utils::getNewID(),nowDisplayingImg);
		images.push_back(img);
		return img;
	}
	else {
		return nullptr;
	}
}

autopilot::Model::Model(QGraphicsView * view, QLabel * labelNavigationStatus, QWidget * window)
{
	this->pView = view;
	this->labelNavigationStatus = labelNavigationStatus;
	setSettingPath(Utils::getSettingsFolder() + "settings.json");
	readSettings();
	ViewInit(window);
	//测试
	//SURFTest();
}

void autopilot::Model::carMoveForward(bool flag)
{
	Utils::log(false, std::string("Car move forward:") + (flag ? "true" : "false"));
	if (isConnected() == true) { //指令发送端
		if (flag == true) {
			state.nowLength = 0;
			state.exceptedLength = 200;
			state.s = carState::forward;
			serialWrite("F0200");
		}
		else {
			serialWrite("S0000");
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
			serialWrite("B0200");
		}
		else {
			serialWrite("S0000");
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
			serialWrite("L0360");
		}
		else {
			serialWrite("S0000");
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
			serialWrite("R0360");
		}
		else {
			serialWrite("S0000");
		}
	}
}

void autopilot::Model::setFlushTimer()
{
	if (flushTimer != nullptr) flushTimer->start();
}

void autopilot::Model::flushView()
{
	flushCarViewRotation(false);
	flushCarViewPosition(false);

	if (state.s == carState::stopped) {
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
	}
	else if (state.s == carState::shutdown) {
		Utils::log(false, "flushView: car shutdown.");
	}
}

void autopilot::Model::setNavigationNode()
{
	//添加图标
	auto point = new QGraphicsPixmapItem(QPixmap(Utils::getUIFolder() + "pathPoint.png"));
	point->setPos(car->pos());
	pView->scene()->addItem(point);
	int newID = Utils::getNewID();
	auto IDItem = new QGraphicsTextItem(QString::fromStdString(std::to_string(newID)));
	point->setPos(QPointF(car->pos().x(),car->pos().y() - 20));
	pView->scene()->addItem(IDItem);
	if (isNowDrawingPath == true) {
		//如果正在配置目标点,则结束这段配置
		nowPath->pathEnd(newID);
		paths.push_back(nowPath);
	}
	else {
		//如果没有配置目标点，则新增目标点
		ViewPoint direct;
		direct.setRotationDeg(state.direction);
		nowPath = new ViewPath(state.pos,newID,direct);
	}
}

void autopilot::Model::startAutoNavigation(int pointID)
{
}

bool autopilot::Model::connectBlueToothSerial()
{
	if (arduino != nullptr) delete arduino;
	arduino = new SerialPort(getPortName().c_str());
	if (arduino->isConnected() == false) {
		Utils::log(false, "ERROR: Failed to connect Serial:" + getPortName());
	}
	else {
		Utils::log(false, "INFO: serial port connected. COM=" + std::to_string(portNum));
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
	if (arduino->isConnected() == true && arduino->readSerialPort(incomingData, MAX_DATA_LENGTH) > 0) {
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

void autopilot::Model::serialWrite(string str)
{
	if (isConnected() == false || str.empty() == true) {
		Utils::log(true, "Serial is not connected or empty data.");
		return;
	}
	else {
		int size = str.size();
		bool result = arduino->writeSerialPort(str.c_str(), min(size, MAX_DATA_LENGTH));
		if (result == false) {
			Utils::log(true, "serialWrite: failed to write str.");
		}
		else {
			Utils::log(false, "serialWrite: write successfully:" + str);
		}
		
	}
}

void autopilot::Model::readArduinoCmd(QString str)
{
	std::string s = str.toStdString();
	switch (str[0].toLatin1())
	{
	case 'E':
		//停止，重置
		cmdFinished(false); //TODO:这里要根据str[1]来判断
		break;
	case 'F':
		//根据当前车状态判断如何更新数据
		if (state.s == carState::forward) {
			state.nowLength = Utils::convert<int>(s.substr(1, s.size()));
		} 
		else if (state.s == carState::backward) {
			state.nowLength = -1 * Utils::convert<int>(s.substr(1, s.size()));
		}
		flushCarViewPosition(false);
		if (isNowDrawingPath == true) {
			nowPath->setNowStepLength(state.nowLength);
		}
		break;
	case 'R':
		if (state.s == carState::turnLeft) {
			state.nowRotation = Utils::convert<int>(s.substr(1, s.size()));
		}
		else if (state.s == carState::turnRight) {
			state.nowRotation = -1 * Utils::convert<int>(s.substr(1, s.size()));
		}
		flushCarViewRotation(false);
		if (isNowDrawingPath == true) {
			nowPath->setNowStepRotation(state.nowRotation);
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