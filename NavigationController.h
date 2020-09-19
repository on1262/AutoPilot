#pragma once
#include "qvector.h"
#include "ViewPath.h"

/*一次导航的控制器
 *从起点到终点，经过一个或多个cachePath
 *每次导航需要给定cachePath列表和当前位置方向，外部在执行E0时调用下一步
 */

namespace autopilot {
	class NavigationController
	{
	public:
		QString getNextCmd(); //导航接收指令后的下一操作
		void newRoute(QVector<ViewPath*> route, ViewPoint nowRealPos, float nowDirection);
		void startNavigation();
		void setEnableFlag(bool flag);
		bool getNavigationState();
	private:
		bool isEnableFlag = false; //当前小车是否稳定
		int TCount = 0;
		int nowRunningCmdIndex = 0;
		bool isNowNavigating = false; //是否在自动导航状态
		QVector<QString> cmdList; //存储导航所需的指令
	};
}

