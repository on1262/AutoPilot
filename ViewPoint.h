#pragma once
namespace autopilot {
	struct ViewPoint {
	public:
		float x = 0.0f;
		float y = 0.0f;
		float getLength();
		static float getDistance(ViewPoint p1, ViewPoint p2); //��ȡ�������
		static float getDirection(ViewPoint pStart, ViewPoint pEnd);
		float getRotationDeg(); //��ȡ��ת�Ƕ�
		void setRotationDeg(float rotation);
	};
}