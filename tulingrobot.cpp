#include "tulingrobot.h"

TulingRobot::TulingRobot(const QString &key,  QObject *parent):
    AbstractRobot(parent),
    _ptrHttp(std::make_shared<MyHttp>(parent)),
  _key(key)
{
    //connect(_ptrHttp.get(), SIGNAL(readyRead(QNetworkReply*)),
       //                                     this, SLOT(readyReply(QNetworkReply*)));
    _ptrHttp->setPollTime(1000);
}

TulingRobot::~TulingRobot()
{

}

bool TulingRobot::request(const QString &URL,QString const &id,
                            const AbstractRobot::CallBack &cb, Method m)
{
    (void)m;
     auto Base= QString ("http://www.tuling123.com/openapi/api?key=%1&userid=%2&info=").arg(_key).arg(id);
;
     Base.append(URL);
     return _ptrHttp->request(Base, cb, MyHttp::GET, nullptr);
     /*
     auto FullURL = QUrl(Base);

     if (FullURL.isValid()){
         // ~~效率有问题
        //_Handle[FullURL] = cb;
         _Handle[URL] = cb;
        _ptrHttp->get(QNetworkRequest(FullURL));
        return true;
     }

     return false;
     */
}

void TulingRobot::readyReply(QNetworkReply *reply)
{

}


