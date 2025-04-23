#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
#endif
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/font/fonts/Roboto-Regular.ttf");

    MainWindow w;
    w.show();
    return a.exec();
}
