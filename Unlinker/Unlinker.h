#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Unlinker.h"
#include "UnlinkerClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UnlinkerClass; };
QT_END_NAMESPACE

class Unlinker : public QMainWindow
{
    Q_OBJECT

public:
    Unlinker(QWidget *parent = nullptr);
    ~Unlinker();

private:
    Ui::UnlinkerClass *ui;
    UnlinkerClient* m_client;
};
