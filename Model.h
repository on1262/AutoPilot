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
namespace autopilot {
	/*从视角变换中还原的位置移动信息*/
	struct viewVector {
		float x; //水平移动距离
		float y; //垂直移动距离
		float center; //中心放缩距离
	};
	class Model
	{
	public:
		/*小车控制*/
		float rotateAngle = 360.0; //单位旋转角度
		float moveSpeed = 10.0; //前进后退速度

		/*自动导航*/
		QString mapFolderPath; //地图存放文件夹
		bool isLoadTestMapWhenStart = false; //是否在最初的时候打开测试地图

		/*蓝牙串口*/
		bool connectBlueToothSerial();
		bool getCarSerialStatus();
		bool isCarSerialPortActivated = false;
		//char incomingData[MAX_DATA_LENGTH];
		int baudRate = 9600;
		int portNum = 6;
		//SerialPort* arduino = nullptr;
		std::string getPortName();
		void setPortName(int num);
		void listenOnce();
		/*串口信息显示*/
		long bufferSize = 10000;
		int bufferUpdateFrequency = 115200;
		QString getBufferText();

		/*图像识别*/
		QString testFolder;
		QString cacheFolder; //实时图片存储的文件
		int cameraSamplingFrequency = 10; //对视频流采样的频率
		int compressedWidth = 300; //压缩后图片宽度
		int compressedHeight = 400; //压缩后图片高度
		bool isTranslateToBW = false;//是否压缩成灰度图
		viewVector SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath);
		void SURFTest(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles);
		void IPCameraTest();

		/*配置文件*/
		QString settingPath; //配置文件的路径
		void readSettings(); //将设置文件中的内容读取到界面中
		void writeSettings(); //将界面的内容保存到设置文件中
		void setSettingPath(QString settingPath); //改变配置文件的路径

		/*轨迹显示与实时检测*/
		ViewItemCar* car;
		QVector<ViewImage*> images;
		void ViewInit(QWidget* window); //初始化UI界面
		void addViewImage(); //加载一张图片到地图上
		QGraphicsView* pView;
		Model(QGraphicsView* view, QWidget* window);
	};
}
