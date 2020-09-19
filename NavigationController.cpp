#include "NavigationController.h"
QString autopilot::NavigationController::getNextCmd()
{
	QString str;
	if (isNowNavigating == true) {
		if (nowRunningCmdIndex <= cmdList.size() - 1) {
			//如果还有指令，检测小车是否稳定
			if (isEnableFlag == true) {
				//如果小车稳定了,执行下一条指令
				str = cmdList[nowRunningCmdIndex];
				nowRunningCmdIndex++;
				isEnableFlag = false;
				TCount = 0;
			}
			else {
				if (TCount < 6) {
					//如果小车没有稳定,则持续发送查询指令
					str = "T";
					TCount++;
				}
				else {
					str = "A0000"; //如果连续6次查询都是未平衡，则认为是稳态误差，执行A指令强制平衡
					TCount = 0;
				}
			}
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

void autopilot::NavigationController::setEnableFlag(bool flag)
{
	isEnableFlag = flag;
}

bool autopilot::NavigationController::getNavigationState()
{
	return isNowNavigating;
}