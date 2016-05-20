#ifndef QQZONE_H
#define QQZONE_H

#include <QObject>
#include<QLabel>
#include <myhttp.h>
#include <QJSEngine>
#include <QDebug>
#include <memory>
#include <set>

class QQZone: public QObject
{
    Q_OBJECT
private:
     std::shared_ptr<MyHttp> _Http;
     std::shared_ptr<QLabel> _label;
     std::shared_ptr<QList<QNetworkCookie> > _cookies;
     std::shared_ptr<QTimer> _timer;
     std::shared_ptr<std::set<QString> > _doLikeSet;
     QString genSSID(QString prefix = QString());
     void requestCookies();
     void parseCookie();
     QString genG_tk(QString const &p_skey);
     QString cookieString();
     void doLike(QString  &jsonStr);
     void postLikeReq(QString const &uin, QString const &key,
                                    QString const &appid, QString const &type);
private slots:
     void queryQRCode();
     void onTimerPoll();
public:
     explicit QQZone(QObject *parent = 0);
     void requestQRCode();
};
#endif // QQZONE_H
