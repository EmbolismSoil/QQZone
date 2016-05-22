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
    _reply(nullptr),
    _requests(std::make_shared<std::list<MyHttp::__request> >()),
    _timer(std::make_shared<QTimer>())
    //_httpBuf(std::make_shared<QBitArray>())
{
    connect(_timer.get(), SIGNAL(timeout()), this, SLOT(doRequest()));
    _timer->start(500);
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
    if (buf == nullptr)
        _requests->push_back({req, cb, method,  QByteArray()});
    else _requests->push_back({req, cb, method, *buf});
    return true;
}

bool MyHttp::doRequest()
{
    if(_requests->empty())
        return false;

    auto request =  _requests->front();
    auto req = request.req;
    auto method = request.method;
    auto cb = request.cb;
    _requests->pop_front();


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
            auto buf = request.buf;
            _Http->post(req, buf);
        }else{
            return false;
        }
        return true;
    }
    return true;
}

void MyHttp::setPollTime(unsigned int time)
{
    _timer->stop();
    _timer->start(time);
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

