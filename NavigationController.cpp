#include "NavigationController.h"
QString autopilot::NavigationController::getNextCmd()
{
	QString str;
	if (isNowNavigating == true) {
		if (nowRunningCmdIndex <= cmdList.size() - 1) {
			//�������ָ����С���Ƿ��ȶ�
			if (isEnableFlag == true) {
				//���С���ȶ���,ִ����һ��ָ��
				str = cmdList[nowRunningCmdIndex];
				nowRunningCmdIndex++;
				isEnableFlag = false;
				TCount = 0;
			}
			else {
				if (TCount < 6) {
					//���С��û���ȶ�,��������Ͳ�ѯָ��
					str = "T";
					TCount++;
				}
				else {
					str = "A0000"; //�������6�β�ѯ����δƽ�⣬����Ϊ����̬��ִ��Aָ��ǿ��ƽ��
					TCount = 0;
				}
			}
		}
		else {
			//ִ�н���
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
	auto cmds = route[0]->getCommands(nowRealPos, nowDirection); //��һ��Ҫ�����г���nowRealPos����ӵġ�
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
	nowRunningCmdIndex = 0;//����
}

void autopilot::NavigationController::setEnableFlag(bool flag)
{
	isEnableFlag = flag;
}

bool autopilot::NavigationController::getNavigationState()
{
	return isNowNavigating;
}