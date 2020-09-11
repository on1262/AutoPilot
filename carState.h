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
		ViewPoint carRealPos; //С����ʵ����
		void setRealPos(QPointF posf);

		float direction = 0; //˳ʱ��Ϊ����direction���ۼƷ��򣬲�������0-360����
		float nowRotation = 0; //�м���,����
		float nowLength = 0;
		float exceptedRotation = 0; //������������
		float exceptedLength = 0;

	};
}
