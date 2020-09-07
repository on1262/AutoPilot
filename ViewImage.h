#pragma once
#include "qgraphicsitem.h"
#include "Utils.h"
#include "viewVector.h"
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
		QString path = Utils::getCacheFolder();
		int ID;
		float rotation;
		QPoint pos;
		ImageStatus status;
		cv::Mat image;
	public:
		static ViewImage* createFromFile(QString _path, int _ID); //���ļ��д���
		static ViewImage* createFromMat(int _ID, cv::Mat _img); //���ڴ��ͼ���д���
		//ͼƬ���
		void setImage(cv::Mat img); //����ͼƬ
		cv::Mat getImage(); //��ȡͼƬ
		void showImage(); //չʾͼƬ
		//ͼƬ�ļ�
		int getID();
		void setID(int _ID);
		//·����Ϣ��ָ��ʾ�ڵ�ͼ�ϵ�λ�ã�
		QPoint getPosition();
		void setPosition(QPoint pos);
		//��ת��Ϣ������
		void setRotation(float rotation);
		//�ȶ�
		viewVector compare(ViewImage img2);
		//״̬
		void setStatus(ImageStatus sta);
		ImageStatus getStatus();

		//�����ť����бȶ�
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
	};
}