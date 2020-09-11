#include "ViewNode.h"
#include "Utils.h"

void autopilot::ViewNode::init(QGraphicsScene* scene, int ID, ViewPoint realPos, QPointF screenPos, QString displayText)
{
	this->setPixmap(QPixmap(Utils::getUIFolder() + "pathPoint.png"));
	this->setPos(screenPos);
	Utils::setTransfromCenter(this);
	this->realPos = realPos;
	this->ID = ID;
	this->setZValue(1.5f);
	scene->addItem(this);
	auto IDItem = new QGraphicsTextItem(displayText);
	IDItem->setDefaultTextColor(QColor(qRgb(255, 255, 255)));
	IDItem->setPos(QPointF(screenPos.x(), screenPos.y() - 10));
	IDItem->setZValue(3.0f);
	scene->addItem(IDItem); //添加到场景中
}
