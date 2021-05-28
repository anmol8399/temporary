#include "garuda.h"
#include <QtQuick/QQuickView>
#include <QQuickWidget>
#include <QApplication>
#include<QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GARUDA w;
    w.setWindowState(Qt::WindowMaximized);
    gmap locator;
    w.show();
    return a.exec();
}
