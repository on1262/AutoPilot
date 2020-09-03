#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutoPilot.h"

class AutoPilot : public QMainWindow
{
    Q_OBJECT

public:
    AutoPilot(QWidget *parent = Q_NULLPTR);

private:
    Ui::AutoPilotClass ui;
};
