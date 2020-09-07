#pragma once
#include "qgraphicsitem.h"
#include "Utils.h"
#include "viewVector.h"
#include <opencv2/opencv.hpp>
namespace autopilot {
	/*一幅存储在地图上的图像*/
	enum ImageStatus {
		OutOfRange,
		Detected,
		ErrorPos,
		Ignored
	};
	class ViewImage : public QGraphicsPixmapItem
	{
	private:
		QString path = Utils::getCacheFolder();
		int ID;
		float rotation;
		QPoint pos;
		ImageStatus status;
		cv::Mat image;
	public:
		static ViewImage* createFromFile(QString _path, int _ID); //从文件中创建
		static ViewImage* createFromMat(int _ID, cv::Mat _img); //从内存的图像中创建
		//图片相关
		void setImage(cv::Mat img); //更改图片
		cv::Mat getImage(); //获取图片
		void showImage(); //展示图片
		//图片文件
		int getID();
		void setID(int _ID);
		//路径信息（指显示在地图上的位置）
		QPoint getPosition();
		void setPosition(QPoint pos);
		//旋转信息（方向）
		void setRotation(float rotation);
		//比对
		viewVector compare(ViewImage img2);
		//状态
		void setStatus(ImageStatus sta);
		ImageStatus getStatus();

		//点击按钮则进行比对
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
	};
}