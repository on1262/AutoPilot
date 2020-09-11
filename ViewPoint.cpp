#include"ViewPoint.h"
#include "Utils.h"
#include "qmath.h"
using namespace autopilot;

float autopilot::ViewPoint::getDirection(ViewPoint pStart, ViewPoint pEnd)
{
	ViewPoint deltap{ pEnd.x - pStart.x,pEnd.y - pStart.y };
	return deltap.getRotationDeg();
}

float ViewPoint::getRotationDeg()
{
	//�����QT����ת����һ�£�˳ʱ����תΪ��
	if (Utils::floatEqual(getLength(), 0) == true) {
		Utils::log(true, "ViewPoint: illegal direction.");
		return 0.0f;
	}
	float theta = acos(x / this->getLength());
	if (y > 0) {
		theta *= 180.0f / M_PI;
	}
	else {
		theta = 2 * M_PI - theta;
		theta *= 180.0f / M_PI;
	}
	return theta;
}

void ViewPoint::setRotationDeg(float rotation)
{
	//�ӽǶ���Ϣ�л�ԭ
	x = cos(rotation * M_PI / 180.0f);
	y = sin(rotation * M_PI / 180.0f);
}