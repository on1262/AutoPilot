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
	static void logCommands(QVector<QString> cmds); //��ӡ������
	//�궨��
	static double real2ViewCoef; //��ʵ��1cm��Ӧ�������أ�����ȡ20���Ƕ�Ӧ20���ء�
	//����
	static std::string toGBK(std::string str); //unicodeתgbk
	static QTextCodec *codec;
	static QString toUnicode(std::string str); //gbkתunicode
	static void setTransfromCenter(QGraphicsPixmapItem* item);
	static bool floatEqual(float left, float right,float prec = 0.00001);
	static std::string convertFloat(float f); //ɾ������Ҫ��0���������ɹ����ַ���
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

