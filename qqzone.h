#ifndef QQZONE_H
#define QQZONE_H

#include <QObject>
#include<QLabel>
#include <myhttp.h>
#include <QJSEngine>
#include <QDebug>
#include <memory>
#include <set>
#include <tulingrobot.h>

//new feed request = http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/cgi_get_feeds_count.cgi?uin=743703241&rd=0.7037725576665252&g_tk=633031855
//get new feed = http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/feeds2_html_pav_all?uin=743703241&begin_time=0&end_time=0&getappnotification=1&getnotifi=1&has_get_key=0&offset=0&set=1&count=10&useutf8=1&outputhtmlfeed=1&grz=0.6906262156553566&scope=1&g_tk=633031855
class QQZone: public QObject
{
    Q_OBJECT
private:
     std::shared_ptr<MyHttp> _Http;
     std::shared_ptr<QLabel> _label;
     std::shared_ptr<QList<QNetworkCookie> > _cookies;
     std::shared_ptr<QTimer> _timer;
     std::shared_ptr<std::set<QString> > _doLikeSet;
     std::shared_ptr<TulingRobot> _Robot;

     QString genSSID(QString prefix = QString());
     void requestCookies();
     void parseCookie();
     QString genG_tk(QString const &p_skey);
     QString cookieString();
     void doLike(QString  &jsonStr);
     void postLikeReq(QString const &uin, QString const &key,
                                    QString const &appid, QString const &type,
                                     QString const &curKey, QString const &uniKey);
private slots:
     void queryQRCode();
     void onTimerPoll();
     void doRemark(QString const&, QString const&, QString const&);
public:
     explicit QQZone(QObject *parent = 0);
     void requestQRCode();
};
#endif // QQZONE_H
