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
	this->stepStatus = rotating; //��תֻ�����ʼ��ʱ�����
}

float ViewPathStep::getLength()
{
	return length + updatedLength;
}

void ViewPathStep::flush()
{
	//ˢ����ʾ�����
	if (Utils::floatEqual(rotationVec.getLength(), 0) == false) {
		//����
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

void ViewPath::addStep(QPointF newStepScreenPos, ViewPoint newStepRealPos)
{
	//��ǰ��ƫ������һ���ĽǶ�
	ViewPathStep* step = new ViewPathStep(nowStep, nowStep->rotationVec);
	//��һ��
	nowStep->nextRealPos = newStepRealPos;
	nowStep->next = step;
	//��һ��
	nowStep = step;
	step->lastRealPos = newStepRealPos;
	//����²�����Ļ��
	step->setPos(QPointF(newStepScreenPos.x() + 18, newStepScreenPos.y() + 15));
	step->setZValue(1.0f);
	scene->addItem(step);

	flush();
}

void autopilot::ViewPath::addAllStepToScene()
{
	auto step = startStep;
	while (step != nullptr) {
		step->setZValue(1.0f);
		scene->addItem(step);
		nowStep = step;
		flush();
		step = step->next;
	}
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
	//���vec1��Ӧ�ĽǶ�

	float theta = vec1.getRotationDeg();
	//����������Ƕ�
	theta -= rotationDeg; //˳ʱ������������˳ʱ���Ǽ�
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

autopilot::ViewPath::~ViewPath()
{
	auto step = this->startStep;
	ViewPathStep* tmp;
	while (step != nullptr) {
		tmp = step->next;
		delete step;
		step = tmp;
	}
}

ViewPath * ViewPath::getReversePath(ViewPath* path) //��һ��������ɵ�ViewPath��ת�õ�����·��
{
	ViewPath* reversedPath = new ViewPath();
	//��ʼ�������ԵĲ���
	reversedPath->scene = path->scene;
	reversedPath->startID = path->endID;
	reversedPath->endID = path->startID;
	//�����һ����ʼ��ǰ
	ViewPathStep* step = path->endStep;
	ViewPathStep* reversedStep = nullptr;
	while (step != nullptr) {
		ViewPoint reverseDirection = step->rotationVec;
		//��ת180��
		reverseDirection.x = -reverseDirection.x;
		reverseDirection.y = -reverseDirection.y;
		//��ӵ�·��
		if (step == path->startStep) { //���ִ��

			reversedStep = new ViewPathStep(reversedStep, reverseDirection); //��������last
			reversedStep->last->next = reversedStep; //������һ��next
			reversedStep->next = nullptr; //��������next
			reversedPath->endStep = reversedStep;
		} else if (step == path->endStep) { //һ��ʼִ��
			reversedStep = new ViewPathStep(nullptr, reverseDirection); //��������last
			reversedPath->startStep = reversedStep;
		}
		else { //�м�ִ��
			reversedStep = new ViewPathStep(reversedStep, reverseDirection); //��������last
			reversedStep->last->next = reversedStep; //������һ��next
		}

		reversedStep->stepStatus = ViewPathStep::expanding;
		//��ӳ���
		reversedStep->updatedLength = step->getLength();
		//����ǰ��ĶԽ�,ע������ԽӲ��ܶ�step��last��next����Ϊ���ǵķ���������
		reversedStep->lastRealPos = step->nextRealPos;
		reversedStep->nextRealPos = step->lastRealPos;

		//step��ǰ����
		step = step->last;
	}
	return reversedPath;
}

float autopilot::ViewPath::getPathSumLength(ViewPath * path)
{
	float length = 0;
	ViewPathStep* step = path->startStep;
	while (step != nullptr) {
		length += abs(step->getLength());
		step = step->next;
	}
	return length;
}

QVector<QString> autopilot::ViewPath::getCommands(ViewPoint realPos, float rotation)
{
	if (this->startStep == nullptr || this->endStep == nullptr) {
		Utils::log(true, "ViewPath getCommands can't be used by unFinished ViewPath.");
	}
	QVector<QString> *vec = new QVector<QString>();
	ViewPathStep* step = startStep;
	float nowRotation = 0.0f;
	if (Utils::floatEqual(ViewPoint::getDistance(realPos, startStep->lastRealPos), 0, 1.5f) == false) {
		//���������㣬����ֱ�ߵ���Ԥ������
		vec->push_back(getRotateCmd(rotation, ViewPoint::getDirection(realPos, startStep->lastRealPos))); //ת��
		vec->push_back(getMoveCmd(realPos, startStep->lastRealPos, 'F')); //ǰ��
		nowRotation = (ViewPoint::getDirection(realPos, startStep->lastRealPos));
	}
	else {
		nowRotation = rotation;
	}
	while (step != nullptr) {
		//ת��
		vec->push_back(getRotateCmd(nowRotation, step->rotationVec.getRotationDeg()));
		//ֱ��
		vec->push_back(getMoveCmd(step->lastRealPos, step->nextRealPos, 'F'));
		nowRotation = step->rotationVec.getRotationDeg();
		step = step->next;
	}
	//�޳���Ч��
	for (auto i = vec->begin(); i != vec->end(); i++) {
		if (i->isEmpty() == true) vec->erase(i);
	}
	return *vec;
}

QString autopilot::ViewPath::getRotateCmd(float rotationStart, float rotationEnd)
{
	QString str;
	int thetaDelta =  rotationEnd - rotationStart;
	if (thetaDelta == 0) {
		//���û����ת�������Ժ��Բ���
		return str;
	}
	else {
		int deltaAbs = abs(thetaDelta);
		//����������ת
		if (deltaAbs >= 360) deltaAbs -= 360;
		if (deltaAbs == 0) return str;
		//�������ת�ĳ��ڽ���ת
		if (thetaDelta > 0) {
			if (deltaAbs >= 180) {
				str += "L0";
				deltaAbs = 360 - deltaAbs;
			}
			else {
				str += "R0";
			}
		}
		else {
			if (deltaAbs >= 180) {
				str += "R0";
				deltaAbs = 360 - deltaAbs;
			}
			else {
				str += "L0";
			}

		}
		//����λ��0
		if (deltaAbs <= 99) {
			str += "0";
		}
		if (deltaAbs <= 9) {
			str += "0";
		}
		str += QString::number(deltaAbs);
		//�Ϸ��Լ��
		if (str.size() != 5) {
			Utils::log(true, "getRotate cmd: Illegal rotation cmd.");
		}
		return str;
	}
}

QString autopilot::ViewPath::getMoveCmd(ViewPoint pStartReal, ViewPoint pEndReal, char mode)
{
	QString str;
	if (mode == 'F' || mode == 'B') {
		str += mode;
		str += "0"; //TODO������Ҫ�ĳɸߵ͵�λ�Զ��ж�
	}
	else {
		Utils::log(true, "getMoveCmd: Illegal direction");
	}
	int dist = abs(ViewPoint::getDistance(pStartReal, pEndReal));
	if (dist == 0) return QString(); //���û�о��룬��ʲôҲ����
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