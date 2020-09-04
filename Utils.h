#pragma once
#include "qstring.h"
class Utils
{
private:
	static QString data;
public:
	//�ļ���λ�ô��
	static QString getDataFolder();
	static QString getUIFolder();
	static QString getMapFolder();
	static QString getSettingsFolder();
	static bool floatEqual(float left, float right,float prec = 0.00001);
	static void log(bool isError, std::string str);
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

