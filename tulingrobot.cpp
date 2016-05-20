#include "tulingrobot.h"

TulingRobot::TulingRobot(const QString &key, const QString &id, QObject *parent):
    AbstractRobot(parent),
    _ptrHttp(std::make_shared<QQHttp>(parent))
{
    _TulingURL = QString ("http://www.tuling123.com/openapi/api?key=%1&userid=%2&info=").arg(key).arg(id);
    connect(_ptrHttp.get(), SIGNAL(readyRead(QNetworkReply*)),
                                            this, SLOT(readyReply(QNetworkReply*)));
}

TulingRobot::~TulingRobot()
{

}

bool TulingRobot::request(const QString &URL,
                            const AbstractRobot::CallBack &cb, Method m)
{
    (void)m;
     auto Base= _TulingURL;
     Base.append(URL);
     auto FullURL = QUrl(Base);

     if (FullURL.isValid()){
         // ~~效率有问题
        //_Handle[FullURL] = cb;
         _Handle[URL] = cb;
        _ptrHttp->get(QNetworkRequest(FullURL));
        return true;
     }

     return false;
}

void TulingRobot::readyReply(QNetworkReply *reply)
{
    setReply(reply);
    auto url  = reply->request().url();
    auto key = url.toString().split("info=");
    if (key.size() > 1)
        EventHandle(key[1]);
}


