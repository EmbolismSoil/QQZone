#ifndef QQHTTP_H
#define QQHTTP_H

#include <QObject>
#include <memory>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QString>

class QQHttp : public QObject
{
    Q_OBJECT
private:
    std::shared_ptr<QNetworkAccessManager> _NetWorkManager;
public:
    explicit QQHttp(QObject *parent = 0);

signals:

public slots:
     virtual void get(const QNetworkRequest &info);
     virtual void post(const QNetworkRequest& req, const  QByteArray& a);
signals:
    void readyRead(QNetworkReply *);
private slots:
    virtual void __readyRead(QNetworkReply *);
};

#endif // QQHTTP_H
