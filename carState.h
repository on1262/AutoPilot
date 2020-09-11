#pragma once
#include "viewVector.h"
#include "ViewPoint.h"
#include "qpoint.h"
namespace autopilot {

	class carState {
	public:
		enum state {
			forward, backward, turnLeft, turnRight, stopped, shutdown, unDetected
		};
		state s = unDetected;
		ViewPoint carRealPos; //小车现实坐标
		void setRealPos(QPointF posf);

		float direction = 0; //顺时针为正，direction是累计方向，不限制在0-360度内
		float nowRotation = 0; //中间量,增量
		float nowLength = 0;
		float exceptedRotation = 0; //最终量，增量
		float exceptedLength = 0;

	};
}
