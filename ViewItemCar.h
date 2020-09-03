#pragma once
#include "qgraphicsitem.h"
/*小车的抽象类*/
class ViewItemCar : public QGraphicsPixmapItem
{
private:
	QPoint pos;
public:
	void init(QPoint pos);
};

