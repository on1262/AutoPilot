#pragma once
#include "qgraphicsitem.h"
#include "Utils.h"
#include "ViewVector.h"
#include "ViewPoint.h"
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
		QString saveFileName;
		ImageStatus status;
		cv::Mat image;
	public:
		int imageID;
		int nodeID;
		float rotation;
		ViewPoint realPos;
		//创建
		static ViewImage* createFromFile(QString _path, int _ID); //从文件中创建
		static ViewImage* createFromMat(int _ID, cv::Mat _img); //从内存的图像中创建
		//保存
		QString saveAsCache(); //存储在cache里
		//图片相关
		void setImage(cv::Mat img); //更改图片
		cv::Mat getImage(); //获取图片
		void showImage(); //展示图片
		//设置
		void init(int imageID, int nodeID,float rotation);
		//状态
		void setStatus(ImageStatus sta);
		ImageStatus getStatus();

		//点击按钮则进行比对
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
	};
}