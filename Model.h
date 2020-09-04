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
namespace autopilot {
	/*从视角变换中还原的位置移动信息*/
	struct viewVector {
		float x; //水平移动距离
		float y; //垂直移动距离
		float center; //中心放缩距离
	};
	struct carState {
		enum state {
			forward, backward, turnLeft, turnRight, stopped, shutdown, unDetected
		};

		state s = unDetected;
		ViewPoint pos; //小车现实坐标
		float direction = 0; //原先的方向
		float nowRotation = 0; //中间量,增量
		float nowLength = 0;
		float exceptedRotation = 0; //最终量，增量
		float exceptedLength = 0;
	};

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
		/*自动导航*/
		carState state; //小车当前状态
		QVector<ViewPath*> paths;
		ViewPath* nowPath;
		bool isNowConfiguringPath = false;
		QString mapFolderPath; //地图存放文件夹
		bool isLoadTestMapWhenStart = false; //是否在最初的时候打开测试地图
		void setNavigationNode(); //设置导航点
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
		void serialWrite(QString str);
		void readArduinoCmd(QString str);
		bool isConnected();
		/*图像识别*/
		QString testFolder;
		QString cacheFolder; //实时图片存储的文件
		int cameraSamplingFrequency = 10; //对视频流采样的频率
		int compressedWidth = 300; //压缩后图片宽度
		int compressedHeight = 400; //压缩后图片高度
		bool isTranslateToBW = false;//是否压缩成灰度图
		viewVector SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath);
		void SURFMutiFiles(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles);
		void SURFTest();
		void IPCameraTest();

		/*配置文件*/
		QString settingPath; //配置文件的路径
		void readSettings(); //将设置文件中的内容读取到界面中
		void writeSettings(); //将界面的内容保存到设置文件中
		void setSettingPath(QString settingPath); //改变配置文件的路径

		/*轨迹显示与实时检测*/
		double real2ViewCoef = 20; //现实中1cm对应多少像素，这里取20就是对应20像素。
		ViewItemCar* car;
		QVector<ViewImage*> images;

		void flushCarViewPosition(); //根据state刷新
		void flushCarViewRotation();
		/**
		 * 坐标说明
		 * 屏幕坐标和小车坐标是两套坐标，屏幕坐标以左上角为原点，单位长度对应一个像素
		 * 小车坐标以小车初始位置为原点，单位长度对应现实中的1cm，两个坐标的旋转的解释是一样的。
		 * 默认情况下小车现实坐标原点与画布中心重合
		**/
		void ViewInit(QWidget* window); //初始化UI界面
		void addViewImage(); //加载一张图片到地图上
		QGraphicsView* pView;
		Model(QGraphicsView* view, QWidget* window);
	};
}
