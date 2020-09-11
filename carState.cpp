#include "carState.h"

void autopilot::carState::setRealPos(QPointF posf)
{
	carRealPos.x = (float)posf.x();
	carRealPos.y = (float)posf.y();
}

