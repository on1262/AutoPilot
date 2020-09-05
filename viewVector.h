#pragma once
namespace autopilot {
	/*从视角变换中还原的位置移动信息*/
	struct viewVector {
		float x; //水平移动距离
		float y; //垂直移动距离
		float center; //中心放缩距离
	};
}