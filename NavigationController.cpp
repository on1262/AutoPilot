#include "NavigationController.h"

QString autopilot::NavigationController::getNextCmd()
{
	QString str;
	if (isNowNavigating == true) {
		if (nowRunningCmdIndex <= cmdList.size() - 1) {
			str = cmdList[nowRunningCmdIndex];
			nowRunningCmdIndex++;
		}
		else {
			//执行结束
			isNowNavigating = false; 
			Utils::log(false, "Navigation controller: route finished.");
		}
	}
	return str;
}

void autopilot::NavigationController::newRoute(QVector<ViewPath*> route, ViewPoint nowRealPos, float nowDirection)
{

	if (route.isEmpty() == true) {
		Utils::log(true, "NavigationController: not a route.");
		return;
	}
	cmdList.clear();
	auto cmds = route[0]->getCommands(nowRealPos, nowDirection); //第一个要单独列出，nowRealPos是外加的。
	cmdList = cmdList + cmds;
	for (auto i = route.begin() + 1; i != route.end(); i++) {
		cmds = (*i)->getCommands((*(i - 1))->endStep->nextRealPos, (*(i - 1))->endStep->rotationVec.getRotationDeg());
		cmdList = cmdList + cmds;
	}
	cmdList.append(QString("ImageCompare"));
	Utils::logCommands(cmdList); //debug
}

void autopilot::NavigationController::startNavigation()
{
	isNowNavigating = true;
	nowRunningCmdIndex = 0;//重置

}

bool autopilot::NavigationController::getNavigationState()
{
	return isNowNavigating;
}

