#include "ViewPath.h"
#include "Utils.h"
#include "qmath.h"
#include "ViewPoint.h"

using namespace autopilot;
ViewPathStep::ViewPathStep(ViewPathStep* lastStep, ViewPoint _direction)
{
	this->last = lastStep;
	this->next = nullptr;
	this->originPathImg = QPixmap(Utils::getUIFolder() + "pathLine.png");
	this->setPixmap(originPathImg);
	this->setTransformOriginPoint(QPointF(0, this->pixmap().height() / 2.0f));
	this->rotationVec = _direction;
	this->length = 0;
	this->updatedLength = 0;
	this->stepStatus = rotating; //旋转只能在最开始的时候进行
}

float ViewPathStep::getLength()
{
	return length + updatedLength;
}

void ViewPathStep::flush()
{
	//刷新显示的外观
	if (Utils::floatEqual(rotationVec.getLength(), 0) == false) {
		//缩放
		if (getLength() > 0) {
			this->setPixmap(originPathImg.scaled(Utils::real2ViewCoef * getLength(), 10));
			this->setRotation(rotationVec.getRotationDeg());
		}
		else {
			this->setPixmap(originPathImg.scaled(-1.0f * Utils::real2ViewCoef * getLength(), 10));
			this->setRotation(rotationVec.getRotationDeg() + 180.0f);
		}

		this->setVisible(true);
	}
}

float ViewPathStep::getRotationDeg(ViewPathStep * lastStep, ViewPathStep * nextStep)
{
	return lastStep->rotationVec.getRotationDeg() - nextStep->rotationVec.getRotationDeg();
}

void ViewPath::addStep(QPointF p)
{
	//当前的偏角是上一步的角度
	ViewPathStep* step = new ViewPathStep(nowStep, nowStep->rotationVec);
	//上一步
	nowStep->nextRealPos = ViewPoint{ (float)p.x(), (float)p.y() };
	nowStep->next = step;
	//下一步
	nowStep = step;
	step->lastRealPos = ViewPoint{ (float)p.x(), (float)p.y() };
	//添加新步到屏幕上
	step->setPos(QPointF(p.x() + 18, p.y() + 15));
	step->setZValue(1.0f);
	scene->addItem(step);

	flush();
}

void ViewPath::setNowStepRotation(float rotationDeg)
{
	nowStep->rotationVec.setRotationDeg(rotationDeg);
	flush();
}

void ViewPath::setNowStepLength(float length)
{
	nowStep->length = length;
	flush();
}
void ViewPath::updateNowStepLength()
{
	if (nowStep != nullptr) {
		nowStep->updatedLength += nowStep->length;
		nowStep->length = 0;
	}
}
/*
void ViewPath::addRotation(float rotationDeg)
{
	ViewPoint vec1 = nowStep->vec;
	//求出vec1对应的角度

	float theta = vec1.getRotationDeg();
	//添加上新增角度
	theta -= rotationDeg; //顺时针是正，于是顺时针是减
	while (theta > 360.0f || theta < 0) {
		if (theta > 360.0f) theta -= 360.0f;
		if (theta < 0) theta += 360.0f;
	}
	float length = nowStep->vec.getLength();
	vec1.setRotationDeg(theta);
	nowStep->vec.x = vec1.x * length;
	nowStep->vec.y = vec1.y * length;
}

void ViewPath::addLength(float length)
{
	float k = nowStep->vec.getLength();
	k = (k + length) / k;
	nowStep->vec.x *= k;
	nowStep->vec.y *= k;
}
*/
void ViewPath::pathEnd(ViewPoint endPos, int endID)
{
	this->endID = endID;
	this->endStep = nowStep;
	nowStep->next = nullptr;
	nowStep->nextRealPos = endPos;
	rotationEnd = nowStep->rotationVec;
	rotationStart = startStep->rotationVec;
}

void ViewPath::flush()
{
	if (nowStep != nullptr) nowStep->flush();
}

ViewPath::ViewPath(QPointF startScreenPos, ViewPoint startRealPos, int startID, ViewPoint direction, QGraphicsScene* sc)
{
	this->scene = sc;
	this->startID = startID;
	this->endStep = nullptr;
	rotationStart = direction;
	if (Utils::floatEqual(direction.x * direction.x + direction.y * direction.y, 1.0f) == false) {
		Utils::log(true, "ViewPath: illegal direction vector.");
	}
	startStep = new ViewPathStep(nullptr, direction);
	startStep->setPos(QPointF(startScreenPos.x() + 18, startScreenPos.y() + 15));
	startStep->lastRealPos = startRealPos;
	startStep->setZValue(1.0f);
	scene->addItem(startStep);
	nowStep = startStep;
	flush();
}

ViewPath * ViewPath::getReversePath()
{
	return nullptr;
}

QVector<QString>* autopilot::ViewPath::getCommands(ViewPoint realPos, float rotation)
{
	if (this->startStep == nullptr || this->endStep == nullptr) {
		Utils::log(true, "ViewPath getCommands can't be used by unFinished ViewPath.");
	}
	QVector<QString> *vec = new QVector<QString>();
	ViewPathStep* step = startStep;
	float nowRotation = 0.0f;
	if (Utils::floatEqual(ViewPoint::getDistance(realPos, startStep->lastRealPos), 0, 1.5f) == false) {
		//如果不在起点，先走直线到达预定区域
		vec->push_back(getRotateCmd(rotation, ViewPoint::getDirection(realPos, startStep->lastRealPos))); //转向
		vec->push_back(getMoveCmd(realPos, startStep->lastRealPos, 'F')); //前进
		nowRotation = (ViewPoint::getDirection(realPos, startStep->lastRealPos));
	}
	else {
		nowRotation = rotation;
	}
	while (step != nullptr) {
		//转向
		vec->push_back(getRotateCmd(nowRotation, step->rotationVec.getRotationDeg()));
		//直行
		vec->push_back(getMoveCmd(step->lastRealPos, step->nextRealPos, 'F'));
		step = step->next;
	}
	//剔除无效项
	for (auto i = vec->begin(); i != vec->end(); i++) {
		if (i->isEmpty() == true) vec->erase(i);
	}
	return vec;
}

QString autopilot::ViewPath::getRotateCmd(float rotationStart, float rotationEnd)
{
	QString str;
	int thetaDelta = rotationEnd - rotationStart;
	if (thetaDelta == 0) {
		//如果没有旋转或差异可以忽略不计
		return str;
	}
	else {
		if (thetaDelta > 0)	str += "R0";
		else str += "L0";
		int deltaAbs = abs(thetaDelta);
		if (deltaAbs <= 99) {
			str += "0";
		}
		if (deltaAbs <= 9) {
			str += "0";
		}
		str += QString::number(deltaAbs);
		//合法性检测
		if (str.size() != 5) {
			Utils::log(true, "getRotate cmd: Illegal rotation cmd.");
		}
		return str;
	}
}

QString autopilot::ViewPath::getMoveCmd(ViewPoint pStart, ViewPoint pEnd, char mode)
{
	QString str;
	if (mode == 'F' || mode == 'B') {
		str += mode;
		str += "0"; //TODO：这里要改成高低档位自动判断
	}
	else {
		Utils::log(true, "getMoveCmd: Illegal direction");
	}
	int dist = abs(ViewPoint::getDirection(pStart, pEnd));
	if (dist == 0) return QString(); //如果没有距离，则什么也不做
	else {
		if (dist <= 99) str += "0";
		if (dist <= 9) str += "0";
		str += QString::number(dist);
	}
	return str;
}

float ViewPoint::getLength()
{
	return sqrt(x * x + y * y);
}

float autopilot::ViewPoint::getDistance(ViewPoint p1, ViewPoint p2)
{
	return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}