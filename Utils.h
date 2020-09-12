#pragma once
#include "qstring.h"
#include "qtextbrowser.h"
#include "qtextcodec.h"
#include <sstream>
#include "qgraphicsitem.h"
#include "qpoint.h"
#include "ViewPoint.h"
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
	static void logCommands(QVector<QString> cmds); //打印命令组
	//宏定义
	static double real2ViewCoef; //现实中1cm对应多少像素，这里取20就是对应20像素。
	//工具
	static std::string toGBK(std::string str); //unicode转gbk
	static QTextCodec *codec;
	static QString toUnicode(std::string str); //gbk转unicode
	static void setTransfromCenter(QGraphicsPixmapItem* item);
	static bool floatEqual(float left, float right,float prec = 0.00001);
	static std::string convertFloat(float f); //删除不必要的0，避免生成过长字符串
	static void log(bool isError, std::string str);
	static autopilot::ViewPoint QPoint2ViewPoint(QPointF p);
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

