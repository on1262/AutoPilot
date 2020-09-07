#pragma once
#include "qgraphicsitem.h"
#include "ViewImage.h"
struct ViewPoint {
public:
	float x = 0.0f;
	float y = 0.0f;
	float getLength();
	float getRotationDeg(); //��ȡ��ת�Ƕ�
	void setRotationDeg(float rotation);
};

/*һС��·��*/
class ViewPathStep : public QGraphicsPixmapItem
{
public:
	enum pathStepStatus{ rotating, expanding}; //һ���������������׶Σ���ת�׶κ�ֱ�н׶�
	pathStepStatus stepStatus; //TODO: ����״̬�ı�
	ViewPathStep* last, *next; //����ṹ
	ViewPoint vec; //��λ����, �������С������
	float length; //��С�����ƶ�����
	ViewPathStep(ViewPathStep* lastStep, ViewPoint _direction);
	float getLength();
	void flush();
	//����
	//�������֮���ת�����Deg���涨˳ʱ��תΪ������ʱ��Ϊ��
	static float getRotationDeg(ViewPathStep* lastStep, ViewPathStep* nextStep);
};
/*����·��*/
class ViewPath
{
public:
	enum detectMethod{onlyStart,onlyEnd,both,none};
	ViewPoint pStart, pEnd; //��βλ��
	int startID, endID; //��βID
	ViewPathStep* startStep;
	ViewPathStep* nowStep;
	void addStep(); //���һ��
	void setNowStepRotation(float rotationDeg); //�����ǰ������
	void setNowStepLength(float length); //�����������
	void pathEnd(int endID); //����·��
	void flush(); //���Ƶ�ˢ��
	ViewPath(ViewPoint start, int startID, ViewPoint direction);
	static ViewPath* getReversePath(); //��ȡ����·��
};

