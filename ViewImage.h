#pragma once
#include "qgraphicsitem.h"
#include "Utils.h"
#include "ViewVector.h"
#include "ViewPoint.h"
#include <opencv2/opencv.hpp>
namespace autopilot {
	/*һ���洢�ڵ�ͼ�ϵ�ͼ��*/
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
		//����
		static ViewImage* createFromFile(QString _path, int _ID); //���ļ��д���
		static ViewImage* createFromMat(int _ID, cv::Mat _img); //���ڴ��ͼ���д���
		//����
		QString saveAsCache(); //�洢��cache��
		//ͼƬ���
		void setImage(cv::Mat img); //����ͼƬ
		cv::Mat getImage(); //��ȡͼƬ
		void showImage(); //չʾͼƬ
		//����
		void init(int imageID, int nodeID,float rotation);
		//״̬
		void setStatus(ImageStatus sta);
		ImageStatus getStatus();

		//�����ť����бȶ�
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
	};
}