#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("tgtsml"));
    QApplication a(argc, argv);
    MainWindow w(nullptr,&a);
    w.show();
    return a.exec();
}
