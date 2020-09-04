#pragma once
/*��̨�߼��ӿ�*/
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
	/*���ӽǱ任�л�ԭ��λ���ƶ���Ϣ*/
	struct viewVector {
		float x; //ˮƽ�ƶ�����
		float y; //��ֱ�ƶ�����
		float center; //���ķ�������
	};
	struct carState {
		enum state {
			forward, backward, turnLeft, turnRight, stopped, shutdown, unDetected
		};

		state s = unDetected;
		ViewPoint pos; //С����ʵ����
		float direction = 0; //ԭ�ȵķ���
		float nowRotation = 0; //�м���,����
		float nowLength = 0;
		float exceptedRotation = 0; //������������
		float exceptedLength = 0;
	};

	class Model
	{
	public:
		/*С������*/
		float rotateAngle = 360.0; //��λ��ת�Ƕ�
		float moveSpeed = 10.0; //ǰ�������ٶ�
		QTimer *flushTimer;
		void carMoveForward(bool flag);
		void carMoveBackward(bool flag);
		void carTurnLeft(bool flag);
		void carTurnRight(bool flag);
		void setFlushTimer();
		void flushView();
		/*�Զ�����*/
		carState state; //С����ǰ״̬
		QVector<ViewPath*> paths;
		ViewPath* nowPath;
		bool isNowConfiguringPath = false;
		QString mapFolderPath; //��ͼ����ļ���
		bool isLoadTestMapWhenStart = false; //�Ƿ��������ʱ��򿪲��Ե�ͼ
		void setNavigationNode(); //���õ�����
		/*��������*/
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
		/*������Ϣ��ʾ*/
		long bufferSize = 1000;
		int bufferUpdateFrequency = 115200;
		/*����������ʶ��*/
		QString cmdStr;
		int CmdsCount = 0; //���յ���ָ������
		void readBuffer(QString str);
		void serialWrite(QString str);
		void readArduinoCmd(QString str);
		bool isConnected();
		/*ͼ��ʶ��*/
		QString testFolder;
		QString cacheFolder; //ʵʱͼƬ�洢���ļ�
		int cameraSamplingFrequency = 10; //����Ƶ��������Ƶ��
		int compressedWidth = 300; //ѹ����ͼƬ���
		int compressedHeight = 400; //ѹ����ͼƬ�߶�
		bool isTranslateToBW = false;//�Ƿ�ѹ���ɻҶ�ͼ
		viewVector SURF(float matchThreshold, std::string leftFilePath, std::string rightFilePath);
		void SURFMutiFiles(float matchThreshold, std::vector<std::string> leftFiles, std::vector<std::string> rightFiles);
		void SURFTest();
		void IPCameraTest();

		/*�����ļ�*/
		QString settingPath; //�����ļ���·��
		void readSettings(); //�������ļ��е����ݶ�ȡ��������
		void writeSettings(); //����������ݱ��浽�����ļ���
		void setSettingPath(QString settingPath); //�ı������ļ���·��

		/*�켣��ʾ��ʵʱ���*/
		double real2ViewCoef = 20; //��ʵ��1cm��Ӧ�������أ�����ȡ20���Ƕ�Ӧ20���ء�
		ViewItemCar* car;
		QVector<ViewImage*> images;

		void flushCarViewPosition(); //����stateˢ��
		void flushCarViewRotation();
		/**
		 * ����˵��
		 * ��Ļ�����С���������������꣬��Ļ���������Ͻ�Ϊԭ�㣬��λ���ȶ�Ӧһ������
		 * С��������С����ʼλ��Ϊԭ�㣬��λ���ȶ�Ӧ��ʵ�е�1cm�������������ת�Ľ�����һ���ġ�
		 * Ĭ�������С����ʵ����ԭ���뻭�������غ�
		**/
		void ViewInit(QWidget* window); //��ʼ��UI����
		void addViewImage(); //����һ��ͼƬ����ͼ��
		QGraphicsView* pView;
		Model(QGraphicsView* view, QWidget* window);
	};
}
