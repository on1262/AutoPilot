#pragma once
#include "qgraphicsitem.h"
#include "ViewImage.h"
#include "qgraphicsscene.h"
#include "viewVector.h"
#include "ViewPoint.h"

namespace autopilot {
	/*һС��·��*/
	class ViewPathStep : public QGraphicsPixmapItem
	{
	public:
		enum pathStepStatus { rotating, expanding }; //һ���������������׶Σ���ת�׶κ�ֱ�н׶�
		pathStepStatus stepStatus; //TODO: ����״̬�ı�
		ViewPoint lastRealPos, nextRealPos;
		ViewPathStep* last, *next; //����ṹ
		ViewPoint rotationVec; //��λ����, �������С������
		QPixmap originPathImg;
		float length; //��ǰ�ƶ����ڿɱ����
		float updatedLength; //�̶��ƶ�����
		ViewPathStep(ViewPathStep* lastStep, ViewPoint _direction);
		float getLength();
		void flush();
		//����
		//�������֮���ת�����Deg
		static float getRotationDeg(ViewPathStep* lastStep, ViewPathStep* nextStep);
	};
	/*����·��*/
	class ViewPath
	{
	public:
		enum detectMethod { onlyStart, onlyEnd, both, none };
		int startID, endID; //��βID
		ViewPathStep* startStep;
		ViewPathStep* nowStep;
		ViewPathStep* endStep; //����
		QGraphicsScene* scene;
		void addStep(QPointF newStepScreenPos, ViewPoint newStepRealPos); //���һ��
		void setNowStepRotation(float rotationDeg); //�����ǰ������
		void setNowStepLength(float length); //�����������
		void updateNowStepLength();
		void pathEnd(ViewPoint endRealPos, int endID); //����·��
		void flush(); //���Ƶ�ˢ��
		ViewPath(QPointF startScreenPos,ViewPoint startRealPos, int startID, ViewPoint direction, QGraphicsScene* sc);
		ViewPath() {}; //������·���ṩ�Ĺ��캯��
		~ViewPath();
		static ViewPath* getReversePath(ViewPath* path); //��ȡ����·��
		static float getPathSumLength(ViewPath* path); //��ȡ·�����ȵĺ�
		QVector<QString> getCommands(ViewPoint realPos,float rotationBeforeStart); //����·����ȡִ�������б�
		QString getRotateCmd(float rotationStart, float rotationEnd); //��startת��end������
		QString getMoveCmd(ViewPoint pStartReal, ViewPoint pEndReal, char mode);
	};
}

