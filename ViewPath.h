#pragma once
#include "qgraphicsitem.h"
#include "ViewImage.h"
#include "qgraphicsscene.h"
#include "viewVector.h"
#include "ViewPoint.h"

namespace autopilot {
	/*一小段路径*/
	class ViewPathStep : public QGraphicsPixmapItem
	{
	public:
		enum pathStepStatus { rotating, expanding }; //一步的运行有两个阶段：旋转阶段和直行阶段
		pathStepStatus stepStatus; //TODO: 加上状态改变
		ViewPoint lastRealPos, nextRealPos;
		ViewPathStep* last, *next; //链表结构
		ViewPoint rotationVec; //单位向量, 方向代表小车方向
		QPixmap originPathImg;
		float length; //当前移动步内可变距离
		float updatedLength; //固定移动距离
		ViewPathStep(ViewPathStep* lastStep, ViewPoint _direction);
		float getLength();
		void flush();
		//工具
		//检测两步之间的转向度数Deg
		static float getRotationDeg(ViewPathStep* lastStep, ViewPathStep* nextStep);
	};
	/*整个路径*/
	class ViewPath
	{
	public:
		enum detectMethod { onlyStart, onlyEnd, both, none };
		ViewPoint rotationStart, rotationEnd; //首尾位置
		int startID, endID; //首尾ID
		ViewPathStep* startStep;
		ViewPathStep* nowStep;
		ViewPathStep* endStep; //结束
		QGraphicsScene* scene;
		void addStep(QPointF p); //添加一步
		void setNowStepRotation(float rotationDeg); //添加向前的数据
		void setNowStepLength(float length); //添加向后的数据
		void updateNowStepLength();
		void pathEnd(ViewPoint endPos, int endID); //结束路径
		void flush(); //绘制点刷新
		ViewPath(QPointF startScreenPos,ViewPoint startPos, int startID, ViewPoint direction, QGraphicsScene* sc);
		static ViewPath* getReversePath(); //获取反演路径
		QVector<QString>* getCommands(ViewPoint realPos,float rotationBeforeStart); //根据路径获取执行命令列表
		QString getRotateCmd(float rotationStart, float rotationEnd); //从start转向到end的命令
		QString getMoveCmd(ViewPoint pStart, ViewPoint pEnd, char mode);
	};
}

