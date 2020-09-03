#include "AutoPilot.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoPilot w;
    w.show();
    return a.exec();
}
