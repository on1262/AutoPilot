#pragma once
#include "ViewPoint.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
namespace autopilot {
	class ViewNode: public QGraphicsPixmapItem
	{
	public:
		void init(QGraphicsScene* scene,int ID, ViewPoint realPos,QPointF screenPos,QString displayText);
		int ID = 0;
		ViewPoint realPos;

	};
}
