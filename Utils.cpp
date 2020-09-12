#include "Utils.h"

#include <iostream>
std::stringstream Utils::ss;
double Utils::real2ViewCoef = 5;
QTextCodec *Utils::codec = QTextCodec::codecForName("GBK");
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

void Utils::logCommands(QVector<QString> cmds)
{
	log(false, "==Commands==");
	for (auto i = cmds.begin(); i != cmds.end(); i++) {
		log(false, i->toStdString());
	}
	log(false, "====End====");
}

bool Utils::floatEqual(float left, float right, float prec)
{
	if (abs(left - right) < prec) return true;
	else return false;
}

std::string Utils::convertFloat(float f)
{
	std::string str = std::to_string(f);
	for (auto i = str.end() - 1; i != str.begin(); i--) {
		if (*i == '.') { //小数点后都是0时，删掉小数点并退出
			str.erase(i);
			break;
		}
		else if (*i != '0') break; //遇到非0数字，退出
		else {
			str.erase(i);//如果不是小数点且是0，那么删除。
		}
	}
	//检测是否误删除了整数部分
	if (abs(Utils::convert<float>(str) - f) > 1.0f) {
		Utils::log(true, "Error in convertFloat().");
	}
	return str;
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

autopilot::ViewPoint Utils::QPoint2ViewPoint(QPointF p)
{
	return autopilot::ViewPoint{(float)p.x(),(float)p.y()};
}


std::string Utils::toGBK(std::string str)
{
	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");       //获取UTF-8编码对象
	QString  unicode = utf8->toUnicode(str.c_str());              //通过UTF-8编码对象将啊哈转为utf-16

	QTextCodec *gbk = QTextCodec::codecForName("GBK");          //获取GBK编码对象
	QByteArray arr = gbk->fromUnicode(unicode);                   //从Unicode编码转为自身编码类型(GBK)
	return arr.toStdString();
}

QString Utils::toUnicode(std::string str)
{
	return codec->toUnicode(str.c_str());
}

void Utils::setTransfromCenter(QGraphicsPixmapItem* item)
{
	QTransform trans;
	trans.translate(item->pixmap().width() / 2.0f, item->pixmap().height() / 2.0f);
	item->setTransform(trans);
}