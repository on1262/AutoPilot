#include "AutoPilot.h"
#include <QtWidgets/QApplication>
#include "RobustMatcher.h"
#include "Model.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "qdebug.h"
#include "Utils.h"
using namespace cv::xfeatures2d;
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoPilot w;
    w.show();
    return a.exec();

}
//����robustMatch,��Ϊ����Utils�����ļ���·����ͬ����û��д��test����(��һ��test//�ͱ�������ֱ��д������·����