#include "Unlinker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    a.setWindowIcon(QIcon(":/Unlinker/assets/1608708_unlink_icon.png"));
    Unlinker w;
    w.show();
    return a.exec();
}
