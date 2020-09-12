#pragma once
#include "qvector.h"
#include "ViewPath.h"

/*一次导航的控制器
 *从起点到终点，经过一个或多个path
 *每次导航需要给定path列表和当前位置方向，外部在执行E0时调用下一步
 */

namespace autopilot {
	class NavigationController
	{
	public:
		QString getNextCmd(); //导航接收指令后的下一操作
		void newRoute(QVector<ViewPath*> route, ViewPoint nowRealPos, float nowDirection);
		void startNavigation();
		bool getNavigationState();
	private:
		int nowRunningCmdIndex = 0;
		bool isNowNavigating = false; //是否在自动导航状态
		QVector<QString> cmdList; //存储导航所需的指令
	};
}

