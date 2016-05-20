#include <QApplication>
#include <qqzone.h>
#include <QFile>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QQZone qzone;
    qzone.requestQRCode();
    return a.exec();
}
