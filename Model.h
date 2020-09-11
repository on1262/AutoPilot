#pragma once
/*后台逻辑接口*/
#include <qstring.h>
#include <string>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "ViewItemCar.h"
#include "ViewImage.h"
#include "ViewPath.h"
#include "qtimer.h"
#include "SerialPort.hpp"
#include "RobustMatcher.h"
#include "viewVector.h"
#include "qlabel.h"
#include "carState.h"
#include "ViewNode.h"
namespace autopilot {

	/****************项目说明*****************/
	//Zorder:
	//car=2, textItem=3,point(标识点)=1.5,path=1
	class Model
	{
	public:
		/*小车控制*/
		float rotateAngle = 360.0; //单位旋转角度
		float moveSpeed = 10.0; //前进后退速度
		QTimer *flushTimer;
		void carMoveForward(bool flag);
		void carMoveBackward(bool flag);
		void carTurnLeft(bool flag);
		void carTurnRight(bool flag);
		void setFlushTimer();
		void flushView();

		/*蓝牙串口*/
		bool connectBlueToothSerial();
		bool getCarSerialStatus();
		bool isCarSerialPortActivated = false;
		char incomingData[MAX_DATA_LENGTH];
		int baudRate = 9600;
		int portNum = 6;
		SerialPort* arduino = nullptr;
		std::string getPortName();
		void setPortName(int num);
		std::string listenOnce();
		/*串口信息显示*/
		long bufferSize = 1000;
		int bufferUpdateFrequency = 115200;
		/*串口命令传输和识别*/
		QString cmdStr;
		int CmdsCount = 0; //接收到的指令数量
		void readBuffer(QString str);
		void serialWrite(std::string str);
		void readArduinoCmd(QString str);
		bool isConnected();
		/*图像识别*/
		cv::Mat nowDisplayingImg;
		bool isCamConnected = false;
		RobustMatcher b;
		QString testFolder;
		QString mapFolderPath;
		QString cacheFolder; //实时图片存储的文件
		int cameraSamplingFrequency = 10; //对视频流采样的频率
		int compressedWidth = 300; //压缩后图片宽度
		int compressedHeight = 400; //压缩后图片高度
		bool isTranslateToBW = false;//是否压缩成灰度图
		ViewVector SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath);
		void SURFMutiFiles(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles);
		void SURFTest();

		/*配置文件*/
		QString settingPath; //配置文件的路径
		void readSettings(); //将设置文件中的内容读取到界面中
		void writeSettings(); //将界面的内容保存到设置文件中
		void setSettingPath(QString settingPath); //改变配置文件的路径

		/*轨迹显示与自动导航*/
		QLabel* labelNavigationStatus;
		
		carState state; //小车当前状态
		ViewItemCar* car;
		QVector<ViewImage*> images;
		ViewImage*  addViewImageFromNowPos(); //根据当前位置得到一张图片
		void cmdFinished(bool isStoppedByError); //接收到E指令后
		QPointF real2ScreenPos(QPointF realp); //从现实坐标转换虚拟坐标
		void flushCarViewPosition(bool isFlushPos); //根据state刷新
		void flushCarViewRotation(bool isFlushDirection);

		QVector<ViewNode*> nodes; //所有路径点的集合
		QVector<ViewPath*> paths; //所有路径的集合
		ViewPath* nowPath;
		int closestNodeID = -1;//最近的节点，-1代表所有节点都在距离外
		bool isNowDrawingPath = false; //是否在绘制路径
		bool isNowNavigating = false; //是否在自动导航状态
		bool isLoadTestMapWhenStart = false; //是否在最初的时候打开测试地图
		void updateClosestNodeID();
		void addNavigationNode(); //设置导航点
		void startAutoNavigation(int pointID);
		/**
		 * 坐标说明
		 * 屏幕坐标和小车坐标是两套坐标，屏幕坐标以左上角为原点，单位长度对应一个像素
		 * 小车坐标以小车初始位置为原点，单位长度对应现实中的1cm，两个坐标的旋转的解释是一样的。
		 * 默认情况下小车现实坐标原点与画布中心重合
		**/
		void ViewInit(QWidget* window); //初始化UI界面
		QGraphicsView* pView;
		Model(QGraphicsView* view, QLabel* labelNavigationStatus,QWidget* window);
	};
}
