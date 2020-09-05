#pragma once
#include "qstring.h"
#include <sstream>
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
	static bool floatEqual(float left, float right,float prec = 0.00001);
	static void log(bool isError, std::string str);
	static std::stringstream ss;
	template<typename T>
	static T convert(std::string str)
	{
		T t;
		ss = std::stringstream();
		ss << str;
		ss >> t;
		return t;
	}
};

