#include "Utils.h"

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
