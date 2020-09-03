#pragma once
#include "qstring.h"
class Utils
{
private:
	static QString data;
public:
	//文件夹位置存放
	static QString getDataFolder();
	static QString getUIFolder();
	static QString getMapFolder();
	static QString getSettingsFolder();
};

