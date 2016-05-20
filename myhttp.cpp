#include "myhttp.h"

void MyHttp::EventHandle(const QString &key)
{
    auto  iter = _Handles.find(key);
    if (iter != _Handles.end()){
        auto &handlers= iter->second;
        if (handlers())
            _Handles.erase(iter);
        else return;
    }
}

MyHttp::MyHttp(QObject *parent) :
    QObject(parent),
    _Http(std::make_shared<QQHttp>(parent)),
    _reply(nullptr)
    //_httpBuf(std::make_shared<QBitArray>())
{
    connect(_Http.get(), SIGNAL(readyRead(QNetworkReply *)), this,
            SLOT(onHttpFinished(QNetworkReply *)));
}

bool MyHttp::request(const QString &url, const MyHttp::CallBack &cb, MyHttp::Method method, const QByteArray *buf)
{
    return request(QNetworkRequest(url), cb, method, buf);
}

bool MyHttp::request(const QNetworkRequest &req, const MyHttp::CallBack &cb,
                                    MyHttp::Method method, const QByteArray *buf)
{
    if(req.url().isValid()){
        auto iter = _Handles.find(req.url().toString());
        if (iter != _Handles.end()){
               return false;
        }else{            
            _Handles[req.url().toString()] = cb;
        }

        if(method == GET){
            _Http->get(req);
        }else if (method == POST){
            _Http->post(req, *buf);
        }else{
            return false;
        }
        return true;
    }
    return true;
}

void MyHttp::setReply(QNetworkReply *r)
{
    _reply = r;
}

QNetworkReply *MyHttp::getReply()
{
    return _reply;
}

void MyHttp::onHttpFinished(QNetworkReply *reply)
{
    setReply(reply);
    EventHandle(QString(reply->url().toString()));
}

