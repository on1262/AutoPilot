#pragma once
namespace autopilot {
	struct ViewPoint {
	public:
		float x = 0.0f;
		float y = 0.0f;
		float getLength();
		static float getDistance(ViewPoint p1, ViewPoint p2); //获取两点距离
		static float getDirection(ViewPoint pStart, ViewPoint pEnd);
		float getRotationDeg(); //获取旋转角度
		void setRotationDeg(float rotation);
	};
}