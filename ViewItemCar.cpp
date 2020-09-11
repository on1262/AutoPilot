#include "ViewItemCar.h"
#include "Utils.h"
void ViewItemCar::init(QPointF pos)
{
	this->setPixmap(QPixmap(Utils::getUIFolder() + "carPos.png"));
	this->setTransformOriginPoint(QPointF(this->pixmap().width() / 2.0f, this->pixmap().height() / 2.0f));
	this->setPos(pos);
}
