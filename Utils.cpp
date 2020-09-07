#include "Utils.h"
#include <iostream>
std::stringstream Utils::ss;
int Utils::nowID = 0;
QTextBrowser* Utils::debugBrowser = nullptr;
QString Utils::data("F:\\Project\\2020SummerHardwareDesign\\APGUI\\AutoPilot\\Data\\");
QString Utils::getDataFolder()
{
	return data;
}

QString Utils::getUIFolder()
{
	return data + "UI\\";
}

QString Utils::getMapFolder()
{
	return data + "map\\";
}

QString Utils::getSettingsFolder()
{
	return data + "settings\\";
}

int Utils::getNewID()
{
	nowID++;
	return nowID;
}

void Utils::setNewIDStartCount(int count)
{
	nowID += count;
}

QString Utils::getCacheFolder()
{
	return data + "cache\\";
}

void Utils::setDebugBrowser(QTextBrowser * browser)
{
	Utils::debugBrowser = browser;
}

bool Utils::floatEqual(float left, float right, float prec)
{
	if (abs(left - right) < prec) return true;
	else return false;
}

void Utils::log(bool isError, std::string str)
{
	if (debugBrowser == nullptr) {
	if (isError) {
		std::cout << "[ERROR]:" << str << std::endl;

	}
	else {
		std::cout  << str << std::endl;
	}
	}
	else {
		if (isError) {
			debugBrowser->append(QString::fromStdString("[ERROR]:" + str + "\n"));

		}
		else {
			debugBrowser->append(QString::fromStdString(str + "\n"));
		}
	}

}
