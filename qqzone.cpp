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
#include <regex>
#include <QFile>

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

QQZone::QQZone(QObject *parent):
    _Http(std::make_shared<MyHttp>(parent)),
    _label(std::make_shared<QLabel>()),
    _cookies(std::make_shared<QList<QNetworkCookie> >()),
    _timer(std::make_shared<QTimer>()),
    _doLikeSet(new std::set<QString>()),
    _Robot(std::make_shared<TulingRobot>("0d03cea08993d83703e7dfa0d31a0b7d")),
    _myNickName()
{    
   QString pgv_info("pgv_info");
   auto pgv_info_value = genSSID("ssid=s");
   QNetworkCookie pgv_pvi_Cookie(pgv_info.toUtf8(), pgv_info_value.toUtf8());
   _cookies->push_back(pgv_pvi_Cookie);

   QString pgv_pvid("pgv_pvid");
   auto pgv_pvid_value = genSSID();
   QNetworkCookie pgv_si_Cookie(pgv_pvid.toUtf8(), pgv_pvid_value.toUtf8());

  _cookies->push_back(pgv_si_Cookie);

     auto cb = [this] {
                         QString pgv_pvi("pgv_pvi");
                         auto pgv_pvi_value = genSSID();
                         QNetworkCookie pgv_pvi_Cookie(pgv_pvi.toUtf8(), pgv_pvi_value.toUtf8());
                         _cookies->push_back(pgv_pvi_Cookie);

                         QString pgv_si("pgv_si");
                         auto pgv_si_value = genSSID("s");
                         QNetworkCookie pgv_si_Cookie(pgv_si.toUtf8(), pgv_si_value.toUtf8());
                        _cookies->push_back(pgv_si_Cookie);

                        QString refer("_qz_referrer");
                        QString referValue("i.qq.com");
                        QNetworkCookie pgv_refer_Cookie(refer.toUtf8(), referValue.toUtf8());

                       _cookies->push_back(pgv_refer_Cookie);
                        parseCookie();
                        QNetworkRequest req(QUrl(QString("http://pingtcss.qq.com/pingd?dm=i.qq.com&pvi=464581632&si=s8405954560&url=/&arg=&ty=&rdm=&rurl=&rarg=&adt=&r2=52955029&r3=-1&r4=1&fl=19.0&scr=1366x768&scl=24-bit&lg=zh-cn&jv=&tz=-8&ct=&ext=adid=&pf=&random=1463636215321")));
                        req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
                        _Http->request(req,  [this]{
                                                            parseCookie();
                                                            connect(_timer.get(), SIGNAL(timeout()), this, SLOT(queryQRCode()));
                                                            _timer->start(3000);
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


        QFile file("/sdcard/DCIM/Camera/QRCode.png");
        file.open( QIODevice::WriteOnly );
        file.resize(0);
        file.write(buf);
        file.close();
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

void QQZone::testFeed(int count)
{
#if 1
    QUrl url(QString("http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/feeds2_html_pav_all?\
uin=%2&begin_time=0&end_time=0&getappnotification=1&getnotifi=1&\
has_get_key=0&offset=0&set=1&count=10&useutf8=1&outputhtmlfeed=1&scope=1&\
g_tk=%1").arg(genG_tk()).arg(getMyUin()));
     QNetworkRequest req(url);
     req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
     _Http->request(req, [this, count]{
                                         auto reply = _Http->getReply();
                                         auto buf =  reply->readAll();
                                         QString str(buf);
                                         QString pattern("_Callback\\((.*)\\)");
                                         QRegExp rex(pattern);
                                         rex.indexIn(str);
                                         auto jsonString = jsObj2JSOn(rex.cap(1));
                                         //std::cout << jsonString.toStdString() << std::endl;
                                         doReply(jsonString, count);
                                         //std::cout << jsonString.toStdString() << std::endl;
                                         return true;
                                     });
#endif
}

QString QQZone::genSSID(QString prefix)
{
    QJSEngine engine;
    QJSValue func = engine.evaluate("String((Math.round(Math.abs(Math.random() - 1) * 2147483647) * (new Date).getUTCMilliseconds())% 1E10)");
    QString str =  func.toString();
    str = prefix + str;
    return str;
}

QString QQZone::genG_tk()
{
    QNetworkCookie cookie(QByteArray("p_skey"), QByteArray(""));
    auto p_skeyPos = std::find(_cookies->begin(), _cookies->end(), cookie);
    QString p_skey(p_skeyPos->value());
    QJSEngine engine;
    QJSValue func = engine.evaluate("(function(str){var hash = 5381;for (var i = 0,len = str.length; i < len; ++i) hash += (hash << 5) + str.charCodeAt(i);return String(hash & 2147483647);})");
    QJSValueList args;
    args << p_skey;
    QJSValue g_tk = func.call(args);
    //qDebug() << g_tk.toString();
    return g_tk.toString();
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
#ifdef __USER_DEBUG__
            qDebug() << "========================Cookies=====================";
            for (auto &iter : *_cookies){
                qDebug() << QString(iter.toRawForm());
            }
#endif
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
    auto  buf = jsonStr.toUtf8();
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

        //data-curkey=(.*?)data-clicklog
        QRegExp  rexKey("data-curkey=\"(.*)\".*data-clicklog");
        rexKey.setMinimal(true);
        rexKey.indexIn(html);
        QString curLink = rexKey.cap(1);
        rexKey.setPattern("data-unikey=\"(.*)\" ");
        rexKey.indexIn(html);
        QString uniLink = rexKey.cap(1);
        rexKey.setPattern("data-topicid=\"(.*)\" ");
        rexKey.indexIn(html);
        QString topicId = rexKey.cap(1);
        //.*</i>取消赞\\([0-9]*\\)</a>.*
         QRegExp rexDid(".*</i>取消赞\\([0-9]*\\)</a>.*");
#if 1
        if (rexDid.indexIn(html) != -1 || nickName == QString("官方Qzone")){
          //  std::cout << html.toStdString() << std::endl;
            continue;
        }
#else
         if(nickName == QString("地瓜叶爸爸[em]e327840[/em]") || nickName == QString("大眼萌")){
            // std::cout << html.toStdString() << std::endl;
         }else{
             continue;
         }
#endif
        postLikeReq(uin, key, appid, type, curLink, uniLink);
      //  doRemark(uin, topicId, QString("test"));
        QString parttern("<div class=\"f-info\">(.+)</div>");
        QRegExp rex(parttern);
        rex.setMinimal(true);
        rex.indexIn(html);
        auto content = rex.cap(1);
#if 0
        _Robot->request(rex.cap(1), uin ,[this, topicId, uin]{
            auto ptr = _Robot->getReply();
            if (!ptr->isReadable())
                return false;

            QByteArray buf = ptr->readAll();
            auto _Json = QJsonDocument::fromJson(buf);
            if (_Json.isObject()){
                QJsonObject obj = _Json.object();
                if (obj.contains("text")){
                    QString remark = obj["text"].toString();
                    qDebug() << remark;
                    doRemark(uin, topicId, remark);
                }
            }
            return true;
        });
#endif
        std::cout << "nickNmae = " << nickName.toStdString()
                                << "  uin = " << uin.toStdString()
                                 << " content : " << dealContent(content).toStdString()  << std::endl;
    }
    return ;
}

void QQZone::postLikeReq(const QString &uin, const QString &key,
                                        const QString &appid, const QString &type,
                                        QString const &curKey, QString const &uniKey)
{
    Q_UNUSED(uin);
    auto opuin = getMyUin();
    QString url = QString ("http://w.qzone.qq.com/cgi-bin/likes/internal_dolike_app?g_tk=%1").arg(genG_tk());
    QNetworkRequest req(url);
    auto table = QString("qzreferrer=http://user.qzone.qq.com/%1&opuin=%1&\
unikey=%2&curkey=%3&form=1&appid=%4&\
typeid=%5&fid=%6&active=0&fupdate=1").arg(opuin).arg(uniKey).arg(curKey).arg(appid).arg(type).arg(key);
    //std::cout << table.toStdString() << std::endl;
    auto content = table.toUtf8();
    auto contentLength = content.length();
    req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
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
    req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
            _Http->request(req, [this]{
                                              auto reply = _Http->getReply();
                                              auto buf = reply->readAll();
                                             // qDebug() << QString(buf);
                                              QString pattern("ptuiCB.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*'(.*)'.*");
                                              QRegExp rex(pattern);
                                              int pos = 0;
                                              rex.indexIn(buf, pos);

                                              if(rex.cap(5) == "登录成功！"){
                                                  _myNickName = rex.cap(6);
                                                  dealNickName(_myNickName);
                                                  std::cout << _myNickName.toStdString() << " 登录成功！"<< std::endl;
                                                  _timer->stop();
                                                  disconnect(_timer.get(), SIGNAL(timeout()), this, SLOT(queryQRCode()));
                                                  QUrl url(rex.cap(3));
                                                  QNetworkRequest req(url);
                                                  req.setRawHeader(QByteArray("Cookie"),cookieString().toUtf8());

                                                  _Http->request(req, [this]{
                                                                                         parseCookie();
                                                                                         connect(_timer.get(), SIGNAL(timeout()), this, SLOT(pollForNewFeed()));
                                                                                       //  connect(_timer.get(), SIGNAL(timeout()), this, SLOT(onTimerPoll()));
                                                                                         _timer->start(1000*5);
                                                                                         _Http->setPollTime(1000*2);
                                                                                         return true;
                                                                           });
                                              }
                                             return true;
                                     });
    //  qDebug() << url;
}

void QQZone::onTimerPoll()
{
     auto uin = getMyUin();
     //QString uin(fullUin.begin()+2, fullUin.end());
     QUrl url(QString("http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/feeds3_html_more?uin=%1&scope=0&view=1&daylist=&\
uinlist=&gid=&flag=1&filter=all&applist=all&refresh=0&aisortEndTime=0&aisortOffset=0&getAisort=0&aisortBeginTime=0&pagenum=1&\
externparam=&firstGetGroup=0&icServerTime=0&mixnocache=0&scene=0&begintime=0&count=10&dayspac=0&sidomain=ctc.qzonestyle.gtimg.cn&\
useutf8=1&outputhtmlfeed=1&getob=1&g_tk=%2").arg(uin).arg(genG_tk()));
    // qDebug() << url.toString();
     QNetworkRequest req(url);
     req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());

    if(req.url().isValid())
         _Http->request(req, [this]{
                                         auto reply = _Http->getReply();
                                         auto buf = reply->readAll();
                                         auto strBuf = std::make_shared<QString>(buf);
                                         //qDebug() << *strBuf;
                                         QString pattern("_Callback\\((.*)\\)");
                                         QRegExp rex(pattern);
                                         rex.indexIn(*strBuf);
                                         auto jsonString = jsObj2JSOn(rex.cap(1));
                                         doLike(jsonString);
                                         return true;
     });
}

QString QQZone::jsObj2JSOn(QString const &str)
{
    QJSEngine engine;
    QString directive("(function(){var obj = ");
    directive =  directive +str + ";";
    directive =  directive + "return  JSON.stringify(obj);})";
    auto value = engine.evaluate(directive);
    auto jsonString = value.call().toString();

    return jsonString;
}

QString &QQZone::dealContent(QString &content)
{
    QRegExp rex;
    rex.setMinimal(true);
    rex.setPattern("<img src=.*/>");
    while(rex.indexIn(content) != -1)
        content.replace(rex, QString(""));
    return content;
}

QString &QQZone::dealNickName(QString &nickName)
{
    QRegExp rex;
    rex.setMinimal(true);
    rex.setPattern("\\[em\\].*\\[/em\\]");
    while(rex.indexIn(nickName) != -1)
        nickName.replace(rex, QString(""));
    return nickName;
}

void QQZone::pollForNewFeed()
{
    auto urlStr = QString("http://user.qzone.qq.com/p/ic2.s21/cgi-bin/feeds/cgi_get_feeds_count.cgi?\
uin=%1&g_tk=%2").arg(getMyUin()).arg(genG_tk());
    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
    _Http->request(req, [this]{
                                        auto reply = _Http->getReply();
                                        auto buf = reply->readAll();
                                        auto strBuf = std::make_shared<QString>(buf);
                                        //qDebug() << *strBuf;
                                        QString pattern("callback\\((.*)\\)");
                                        QRegExp rex(pattern);
                                        rex.indexIn(*strBuf);
                                        auto jsonString = jsObj2JSOn(rex.cap(1));
                                      //  std::cout << jsonString.toStdString() << std::endl;
                                        auto  jsonbuf = QByteArray(jsonString.toUtf8());
                                        auto  jsonDoc = QJsonDocument::fromJson(jsonbuf);
                                        if (!jsonDoc.isObject())
                                            return true;
                                        QJsonValue data = jsonDoc.object()["data"];
                                        if (!data.isObject())
                                            return true;
                                        auto countReply =  data.toObject()["replyHostFeeds_new_cnt"].toInt();
                                        auto countNewFeed = data.toObject()["friendFeeds_new_cnt"].toInt();
                                        if (countReply > 0)
                                             testFeed(countReply);
                                        if (countNewFeed > 0)
                                            onTimerPoll();
                                        return true;
                                    });
}

QString QQZone::getMyUin()
{
    QNetworkCookie cookie(QByteArray("uin"), QByteArray(""));
     auto uinPos = std::find(_cookies->begin(), _cookies->end(), cookie);
     QString fullUin(uinPos->value());
     QString uin;
     auto pos = fullUin.begin() + 1;
     if (*pos != '0')
         uin.push_back(*pos);
     for (++pos; pos != fullUin.end(); ++pos){
         uin.push_back(*pos);
     }
     return uin;
}

void QQZone::doRemark(QString const &hostUin,QString const &topicId,
                                        QString const &words, QString const &commitId,
                                        QString const &commitUin)
{
  //  Q_UNUSED(commitUin);
    auto opuin = getMyUin();
    QString table;
    QUrl url(QString("http://taotao.qzone.qq.com/cgi-bin/emotion_cgi_re_feeds?g_tk=%1").arg(genG_tk()));
    if(commitId == QString()){
        auto tableNC = QString("qzreferrer=http://user.qzone.qq.com/%1&topicId=%2&\
feedsType=100&inCharset=utf-8&outCharset=utf-8&plat=qzone&source=ic&\
hostUin=%3&isSignIn=&platformid=&uin=%1&format=fs&ref=feeds&content=%4&\
richval=&richtype=&private=0&paramstr=1").arg(opuin).arg(topicId).arg(hostUin).arg(words);
        table = tableNC;
       // std::cout << table.toStdString() << std::endl;
    }else{
        auto tableC = QString("qzreferrer=http://user.qzone.qq.com/%1&topicId=%2&\
feedsType=100&inCharset=utf-8&outCharset=utf-8&plat=qzone&source=ic&\
hostUin=%3&isSignIn=&platformid=&uin=%1&format=fs&ref=feeds&content=%4&\
commentId=%5&commentUin=%6&richval=&richtype=&private=0&paramstr=1").arg(opuin).arg(topicId).arg(hostUin).arg(words).arg(commitId).arg(commitUin);
          table = tableC;
           std::cout << table.toStdString() << std::endl;
    }

     QByteArray content = table.toUtf8();
     auto contentLength = content.length();

    QNetworkRequest req(url);
    req.setRawHeader(QByteArray("Cookie"), cookieString().toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
    _Http->request(req, [this]{return true;}, MyHttp::POST, &content);
//qzreferrer:http://user.qzone.qq.com/743703241
//topicId:  from json
//feedsType:100
//inCharset:utf-8
//outCharset:utf-8
//plat:qzone
//source:ic
//hostUin:2242855368
//isSignIn:
//platformid:52
//uin:743703241
//format:fs
//ref:feeds
//content:赞一个
//richval:
//richtype:
//private:0
    //paramstr:1
}

void QQZone::doReply(const QString &jsonStr,int count)
{
    //std::cout << jsonStr.toStdString() << std::endl;
    auto  buf = jsonStr.toUtf8();
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
        if(count < 1)
            return;
        --count;
        if (!iter.isObject())
            continue;
        QString nickName = iter.toObject()["nickname"].toString();
        QString uin = iter.toObject()["uin"].toString();
        QString key = iter.toObject()["key"].toString();
        QString appid = iter.toObject()["appid"].toString();
        QString type = iter.toObject()["typeid"].toString();   //
        QString html = iter.toObject()["html"].toString();
        if(html.contains(QString("访问了我的主页"))){
            ++count;
            continue;
        }
 //       std::cout << html.toStdString() << std::endl;
       dealNickName(nickName);

        if(type != QString("2") && type != QString("3"))
            continue;

        auto parttern = QString("%1.*</a>&nbsp;回复&nbsp;<a class=\"c_tx q_namecard\".*%2.*&nbsp; :(.*)<div class=\"comments-op\">").arg(nickName).arg(_myNickName);
        QRegExp rex(parttern);
        rex.setMinimal(true);
        QString lastReply;
        int pos = 0;
        while((pos = rex.indexIn(html, pos)) != -1){
            lastReply = rex.cap(1);
            pos += rex.matchedLength();
        }
         if (lastReply == QString()){
             auto remarkParttern = QString("<a class=\"c_tx q_namecard\".*>%1.*</a>&nbsp; :(.*)<div class=\"comments-op\">").arg(nickName);
             rex.setPattern(remarkParttern);
             pos = 0;
             while((pos = rex.indexIn(html,pos)) != -1){
                 lastReply = rex.cap(1);
                 pos += rex.matchedLength();
             }
         }

        if(lastReply.size() < 1)
            continue;
        auto Info = lastReply;
        //std::cout << "nickname = " << nickName.toStdString() << " typeid =  " << type.toStdString() << std::endl;
        auto content = lastReply;
        dealContent(content);
        rex.setPattern("data-topicid=\"(.*)\" ");
        rex.indexIn(html);
        QString topicId = rex.cap(1);
        std::cout << nickName.toStdString() << " : " << content.toStdString() << std::endl;
        //t2_tid=[0-9]
        rex.setPattern("t2_tid=([0-9]*)&amp");
        rex.indexIn(html);
        QString commitId = rex.cap(1);
        //std::cout << "html = " << html.toStdString() << "commitId = " << commitId.toStdString() << std::endl;
        //t1_uin=(.*)&amp
        rex.setPattern("t1_uin=(.*)&amp");
        rex.indexIn(html);
        auto HostUin = rex.cap(1);
        rex.setPattern("t2_uin=(.*)&amp");
        rex.indexIn(html);
        auto commentUin = rex.cap(1);

        _Robot->request(Info, uin ,[this, topicId, uin, nickName,commitId, HostUin, commentUin]{
            auto ptr = _Robot->getReply();
            if (!ptr->isReadable())
                return false;

            QByteArray buf = ptr->readAll();
            QString remark;
            auto _Json = QJsonDocument::fromJson(buf);
            if (_Json.isObject()){
                QJsonObject obj = _Json.object();
                if (obj.contains("text")){
                     remark = obj["text"].toString();
                    qDebug() << remark;
                }
            }
            if (remark.contains(QString("相关新闻")))
                    remark = QString("哎呀，咱们聊点别的吧");
            auto words = QString("@{uin:%1,nick:%2,auto:1} %3").arg(uin).arg(nickName).arg(remark);
            doRemark(HostUin, topicId, words, commitId, commentUin);
            return true;
        });
    }
}
