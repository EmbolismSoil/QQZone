#include "qqzone.h"
#include <QImage>
#include <QLabel>
#include <QNetworkCookie>
#include <algorithm>
#include <QTimer>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <myhttp.h>

QQZone::QQZone(QObject *parent):
    _Http(std::make_shared<MyHttp>(parent)),
    _label(std::make_shared<QLabel>()),
    _cookies(std::make_shared<QList<QNetworkCookie> >()),
    _timer(std::make_shared<QTimer>())
{    
   QString pgv_info("pgv_info");
   auto pgv_info_value = genSSID("ssid=s");
   QNetworkCookie pgv_pvi_Cookie(QByteArray::fromStdString(pgv_info.toStdString()),
                                                        QByteArray::fromStdString(pgv_info_value.toStdString()));
   _cookies->push_back(pgv_pvi_Cookie);

   QString pgv_pvid("pgv_pvid");
   auto pgv_pvid_value = genSSID();
   QNetworkCookie pgv_si_Cookie(QByteArray::fromStdString(pgv_pvid.toStdString()),
                                                      QByteArray::fromStdString(pgv_pvid_value.toStdString()));
  _cookies->push_back(pgv_si_Cookie);

     auto cb = [this] {
                         QString pgv_pvi("pgv_pvi");
                         auto pgv_pvi_value = genSSID();
                         QNetworkCookie pgv_pvi_Cookie(QByteArray::fromStdString(pgv_pvi.toStdString()),
                                                                              QByteArray::fromStdString(pgv_pvi_value.toStdString()));
                         _cookies->push_back(pgv_pvi_Cookie);

                         QString pgv_si("pgv_si");
                         auto pgv_si_value = genSSID("s");
                         QNetworkCookie pgv_si_Cookie(QByteArray::fromStdString(pgv_si.toStdString()),
                                                                            QByteArray::fromStdString(pgv_si_value.toStdString()));
                        _cookies->push_back(pgv_si_Cookie);

                        QString refer("_qz_referrer");
                        QString referValue("i.qq.com");
                        QNetworkCookie pgv_refer_Cookie(QByteArray::fromStdString(refer.toStdString()),
                                                                           QByteArray::fromStdString(referValue.toStdString()));
                       _cookies->push_back(pgv_refer_Cookie);
                        parseCookie();
                        QNetworkRequest req(QUrl(QString("http://pingtcss.qq.com/pingd?dm=i.qq.com&pvi=464581632&si=s8405954560&url=/&arg=&ty=&rdm=&rurl=&rarg=&adt=&r2=52955029&r3=-1&r4=1&fl=19.0&scr=1366x768&scl=24-bit&lg=zh-cn&jv=&tz=-8&ct=&ext=adid=&pf=&random=1463636215321")));
                        req.setRawHeader(QByteArray("Cookie"), QByteArray::fromStdString(cookieString().toStdString()));
                        _Http->request(req,  [this]{
                                                            parseCookie();
                                                            connect(_timer.get(), SIGNAL(timeout()), this, SLOT(queryQRCode()));
                                                            _timer->start(2000);
                                                            return true;
                                                 });
                        return true;
                    };
    QString url("http://xui.ptlogin2.qq.com/cgi-bin/xlogin?proxy_url=http%3A//qzs.qq.com/qzone/v6/portal/proxy.html&daid=5&&hide_title_bar=1&low_login=0&qlogin_auto_login=1&no_verifyimg=1&link_target=blank&appid=549000912&style=22&target=self&s_url=http%3A%2F%2Fqzs.qq.com%2Fqzone%2Fv5%2Floginsucc.html%3Fpara%3Dizone&pt_qr_app=%E6%89%8B%E6%9C%BAQQ%E7%A9%BA%E9%97%B4&pt_qr_link=http%3A//z.qzone.com/download.html&self_regurl=http%3A//qzs.qq.com/qzone/v6/reg/index.html&pt_qr_help_link=http%3A//z.qzone.com/download.html");
    _Http->request(url, cb);
}

void QQZone::requestQRCode()
{
    auto cb = [this]{
        auto reply = _Http->getReply();
        auto buf = reply->readAll();
        auto QRCodePng = std::make_shared<QImage>();
        QRCodePng->loadFromData(buf);
        _label->setPixmap(QPixmap::fromImage(*QRCodePng));
        _label->show();
        parseCookie();
        return true;
    };
    QString urlStr("http://ptlogin2.qq.com/ptqrshow?appid=549000912&e=2&l=M&s=3&d=72&v=4&t=0.45682829641737044&daid=5");
    _Http->request(urlStr, cb);
}

QString QQZone::genSSID(QString prefix)
{
    QJSEngine engine;
    QJSValue func = engine.evaluate("String((Math.round(Math.abs(Math.random() - 1) * 2147483647) * (new Date).getUTCMilliseconds())% 1E10)");
    QString str =  func.toString();
    str = prefix + str;
    return str;
}

QString QQZone::genG_tk(const QString &p_skey)
{
    QJSEngine engine;
    QJSValue func = engine.evaluate("(function(str){var hash = 5381;for (var i = 0,len = str.length; i < len; ++i) hash += (hash << 5) + str.charCodeAt(i);return String(hash & 2147483647);})");
    QJSValueList args;
    args << p_skey;
    QJSValue g_tk = func.call(args);
    //qDebug() << g_tk.toString();
    return g_tk.toString();
}

namespace std {
    template <>
       auto find ( QList<QNetworkCookie>::iterator first,
                                    decltype(first) last, const QNetworkCookie& value ) -> decltype(first){
            while(first != last){
                if (first->name() == value.name())
                    return first;
                ++first;
            }
            return last;
       }
}
void QQZone::parseCookie()
{
    auto reply = _Http->getReply();
    auto rawHeader = reply->rawHeaderPairs();
    for (auto &iter : rawHeader){
        if (iter.first == QString("Set-Cookie")){
            auto cookies = QNetworkCookie::parseCookies(iter.second);
            for(auto &iter : cookies){
                auto pos = std::find(_cookies->begin(), _cookies->end(), iter);
                if (pos != _cookies->end()){
                    _cookies->insert(pos, iter);
                    _cookies->erase(pos);
                 }else _cookies->push_back(iter);
            }
            qDebug() << "========================Cookies=====================";
            for (auto &iter : *_cookies){
                qDebug() << QString(iter.toRawForm());
            }
        }
    }
}

QString QQZone::cookieString()
{
    QString cookie;
    for (auto &iter : *_cookies){
        auto str = QString("%1=%2;").arg(QString(iter.name())).arg(QString(iter.value()));
        cookie.append(str);
    }

    return cookie;
}

void QQZone::doLike(QString &jsonStr)
{
    //std::cout << jsonStr.toStdString() << std::endl;
    auto  buf = QByteArray::fromStdString(jsonStr.toStdString());
    auto  jsonDoc = QJsonDocument::fromJson(buf);

    QJsonValue  globalData = (jsonDoc.object())["data"];
    if (!globalData.isObject())
        return;
    auto dataObj  = globalData.toObject();
    QJsonValue data = dataObj["data"];
    if (!data.isArray())
        return;
    QJsonArray dataArray = data.toArray();
    for (auto iter : dataArray){
        if (!iter.isObject())
            continue;
        QString nickName = iter.toObject()["nickname"].toString();
        QString uin = iter.toObject()["uin"].toString();
        QString key = iter.toObject()["key"].toString();
        QString appid = iter.toObject()["appid"].toString();
        QString type = iter.toObject()["typeid"].toString();
        QString html = iter.toObject()["html"].toString();
        //<divclass="f-info">...content</div>
        postLikeReq(uin, key, appid, type);
        QString parttern("<div class=\"f-info\">(.+)</div>");
        QRegExp rex(parttern, Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
        rex.indexIn(html);
        std::string content = rex.cap(1).toStdString();
        auto pos = content.find(std::string("</div>"));
        if(pos != std::string::npos)
            content.erase(content.begin() + pos, content.end());
        std::cout << "nickNmae = " << nickName.toStdString()
                                << "  uin = " << uin.toStdString()
                                 << " content : " << std::endl << content << std::endl;
    }
    return ;
}

void QQZone::postLikeReq(const QString &uin, const QString &key, const QString &appid, const QString &type)
{
    QNetworkCookie uincookie(QByteArray("uin"), QByteArray(""));
     auto uinPos = std::find(_cookies->begin(), _cookies->end(), uincookie);
     QString fullUin(uinPos->value());
     QString opuin;
     for (auto pos = fullUin.begin() + 2; pos != fullUin.end(); ++pos){
         opuin.push_back(*pos);
     }

    QNetworkCookie cookie(QByteArray("p_skey"), QByteArray(""));
    auto p_skeyPos = std::find(_cookies->begin(), _cookies->end(), cookie);
    QString url = QString ("http://w.qzone.qq.com/cgi-bin/likes/internal_dolike_app?g_tk=%1").arg(genG_tk(QString(p_skeyPos->value())));
    QNetworkRequest req(url);
    auto table = QString("qzreferrer=http://user.qzone.qq.com/%1&opuin=%1&\
unikey=http://user.qzone.qq.com/%2/mood/%3&\
http://user.qzone.qq.com/%2/mood/%3&form=1&\
appid=%4&typeid=%5&fid=%3&active=0&fupdate=1").arg(opuin).arg(uin).arg(key).arg(appid).arg(type);
    //std::cout << table.toStdString() << std::endl;
    auto content = QByteArray::fromStdString(table.toStdString());
    auto contentLength = content.length();
    req.setRawHeader(QByteArray("Cookie"), QByteArray::fromStdString(cookieString().toStdString()));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
    _Http->request(req, [this]{return true;}, MyHttp::POST, &content);

    //qzreferrer:http://user.qzone.qq.com/743703241   fix
    //opuin:743703241                            from cookie
    //unikey:http://user.qzone.qq.com/1977791445/mood/d5b3e27526d33d572b110b00
    //curkey:http://user.qzone.qq.com/1977791445/mood/d5b3e27526d33d572b110b00
    //from:1
    //appid:311
    //typeid:0
    //abstime:1463669542
    //fid:d5b3e27526d33d572b110b00   from json
    //active:0
    //fupdate:1
}

void QQZone::queryQRCode()
{
    parseCookie();

    QJSEngine qjs;
    auto action = QString("0-0-%1").arg(qjs.evaluate("String(new Date - 0)").toString());
    QString loginSig;
    for (auto &iter:*_cookies){
        if(QString(iter.name()) == "pt_login_sig"){
            loginSig = QString(iter.value());
        }
    }
    QUrl url(QString("http://ptlogin2.qq.com/ptqrlogin?u1=http://qzs.qq.com/qzone/v5/loginsucc.html?\
para=izone&ptredirect=0&h=1&t=1&g=1&from_ui=1&ptlang=2052&action=%1&js_ver=10157&js_type=1\
&login_sig=%2&pt_uistyle=32&aid=549000912&daid=5&").arg(action).arg(loginSig));
   QNetworkRequest req(url);
    req.setRawHeader(QByteArray("Cookie"), QByteArray::fromStdString(cookieString().toStdString()));
            _Http->request(req, [this]{
                                              auto reply = _Http->getReply();
                                              auto buf = reply->readAll();
                                             // qDebug() << QString(buf);
                                              QString pattern("ptuiCB.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*");
                                              QRegExp rex(pattern);
                                              int pos = 0;
                                              rex.indexIn(buf, pos);
                                              if(rex.cap(5) == "登录成功！"){
                                                  _timer->stop();
                                                  disconnect(_timer.get(), SIGNAL(timeout()), this, SLOT(queryQRCode()));
                                                  QUrl url(rex.cap(3));
                                                  QNetworkRequest req(url);
                                                  req.setRawHeader(QByteArray("Cookie"),
                                                                  QByteArray::fromStdString(cookieString().toStdString()));
                                                  _Http->request(req, [this]{
                                                                                         parseCookie();
                                                                                         connect(_timer.get(), SIGNAL(timeout()), this, SLOT(onTimerPoll()));
                                                                                         _timer->start(1000*5);
                                                                                         return true;
                                                                           });
                                              }
                                             return true;
                                     });
    //  qDebug() << url;
}

void QQZone::onTimerPoll()
{
    QNetworkCookie cookie(QByteArray("uin"), QByteArray(""));
     auto uinPos = std::find(_cookies->begin(), _cookies->end(), cookie);
     cookie.setName(QByteArray("p_skey"));
     auto p_skeyPos = std::find(_cookies->begin(), _cookies->end(), cookie);
     QString fullUin(uinPos->value());
     QString uin;
     for (auto pos = fullUin.begin() + 2; pos != fullUin.end(); ++pos){
         uin.push_back(*pos);
     }
     //QString uin(fullUin.begin()+2, fullUin.end());
     QUrl url(QString("http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/feeds3_html_more?uin=%1&scope=0&view=1&daylist=&\
uinlist=&gid=&flag=1&filter=all&applist=all&refresh=0&aisortEndTime=0&aisortOffset=0&getAisort=0&aisortBeginTime=0&pagenum=1&\
externparam=&firstGetGroup=0&icServerTime=0&mixnocache=0&scene=0&begintime=0&count=10&dayspac=0&sidomain=ctc.qzonestyle.gtimg.cn&\
useutf8=1&outputhtmlfeed=1&getob=1&g_tk=%2").arg(uin).arg(genG_tk(QString(p_skeyPos->value()))));
    // qDebug() << url.toString();
     QNetworkRequest req(url);
     req.setRawHeader(QByteArray("Cookie"),
                     QByteArray::fromStdString(cookieString().toStdString()));
    if(req.url().isValid())
         _Http->request(req, [this]{
                                         auto reply = _Http->getReply();
                                         auto buf = reply->readAll();
                                         auto strBuf = std::make_shared<QString>(buf);
                                         //qDebug() << *strBuf;
                                         QString pattern("_Callback\\((.*)\\)");
                                         QRegExp rex(pattern);
                                         rex.indexIn(*strBuf);
                                         QJSEngine engine;
                                         QString directive("(function(){var obj = ");
                                         directive =  directive + rex.cap(1) + ";";
                                         directive =  directive + "return  JSON.stringify(obj);})";
                                         auto value = engine.evaluate(directive);
                                         auto jsonString = value.call().toString();
                                         doLike(jsonString);
                                         return true;
                             });
}
