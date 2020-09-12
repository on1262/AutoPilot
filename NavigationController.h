#pragma once
#include "qvector.h"
#include "ViewPath.h"

/*һ�ε����Ŀ�����
 *����㵽�յ㣬����һ������path
 *ÿ�ε�����Ҫ����path�б�͵�ǰλ�÷����ⲿ��ִ��E0ʱ������һ��
 */

namespace autopilot {
	class NavigationController
	{
	public:
		QString getNextCmd(); //��������ָ������һ����
		void newRoute(QVector<ViewPath*> route, ViewPoint nowRealPos, float nowDirection);
		void startNavigation();
		bool getNavigationState();
	private:
		int nowRunningCmdIndex = 0;
		bool isNowNavigating = false; //�Ƿ����Զ�����״̬
		QVector<QString> cmdList; //�洢���������ָ��
	};
}

