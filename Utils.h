#pragma once
#include "qstring.h"
#include "qtextbrowser.h"
#include <sstream>
class Utils
{
private:
	static QString data;
public:
	//�ļ���λ�ô��
	static QString getDataFolder();
	static QString getUIFolder();
	static QString getMapFolder();
	static QString getCacheFolder();
	static QString getSettingsFolder();
	//ID
	static int nowID;
	static int getNewID(); //����ID��������ÿ�ε��ö�����һλ
	static void setNewIDStartCount(int count); //��count+1��ʼ�ṩ��ID
	//debug
	static QTextBrowser* debugBrowser;
	static void setDebugBrowser(QTextBrowser* browser);
	//����
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

