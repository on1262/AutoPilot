#pragma once
#include "qstring.h"
#include "qtextbrowser.h"
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
	static QString getCacheFolder();
	static QString getSettingsFolder();
	//ID
	static int nowID;
	static int getNewID(); //递增ID计数器，每次调用都增加一位
	static void setNewIDStartCount(int count); //从count+1开始提供新ID
	//debug
	static QTextBrowser* debugBrowser;
	static void setDebugBrowser(QTextBrowser* browser);
	//工具
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

