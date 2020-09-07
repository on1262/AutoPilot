#pragma once
#include "qgraphicsitem.h"
#include "ViewImage.h"
struct ViewPoint {
public:
	float x = 0.0f;
	float y = 0.0f;
	float getLength();
	float getRotationDeg(); //获取旋转角度
	void setRotationDeg(float rotation);
};

/*一小段路径*/
class ViewPathStep : public QGraphicsPixmapItem
{
public:
	enum pathStepStatus{ rotating, expanding}; //一步的运行有两个阶段：旋转阶段和直行阶段
	pathStepStatus stepStatus; //TODO: 加上状态改变
	ViewPathStep* last, *next; //链表结构
	ViewPoint vec; //单位向量, 方向代表小车方向
	float length; //大小代表移动距离
	ViewPathStep(ViewPathStep* lastStep, ViewPoint _direction);
	float getLength();
	void flush();
	//工具
	//检测两步之间的转向度数Deg，规定顺时针转为正，逆时针为负
	static float getRotationDeg(ViewPathStep* lastStep, ViewPathStep* nextStep);
};
/*整个路径*/
class ViewPath
{
public:
	enum detectMethod{onlyStart,onlyEnd,both,none};
	ViewPoint pStart, pEnd; //首尾位置
	int startID, endID; //首尾ID
	ViewPathStep* startStep;
	ViewPathStep* nowStep;
	void addStep(); //添加一步
	void setNowStepRotation(float rotationDeg); //添加向前的数据
	void setNowStepLength(float length); //添加向后的数据
	void pathEnd(int endID); //结束路径
	void flush(); //绘制点刷新
	ViewPath(ViewPoint start, int startID, ViewPoint direction);
	static ViewPath* getReversePath(); //获取反演路径
};

