#include "qqhttp.h"
#include <QtNetwork/QNetworkRequest>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

QQHttp::QQHttp(QObject *parent):
    QObject(parent)
{
    _NetWorkManager = std::make_shared<QNetworkAccessManager>(parent);
    connect(_NetWorkManager.get(), SIGNAL(finished(QNetworkReply *)), this,
            SLOT(__readyRead(QNetworkReply *)));
}

void QQHttp::get(const QNetworkRequest &url)
{
    _NetWorkManager->get(url);
}

void QQHttp::post(const QNetworkRequest &req, const QByteArray &a)
{
    _NetWorkManager->post(req,a);
}

void QQHttp::__readyRead(QNetworkReply *reply)
{
     emit readyRead(reply);
}
