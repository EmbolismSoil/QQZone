#include <QApplication>
#include <qqzone.h>
#include <QFile>
#include <iostream>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);
    QQZone qzone;
    qzone.requestQRCode();
    return a.exec();
}
