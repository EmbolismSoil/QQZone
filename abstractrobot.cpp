#include "abstractrobot.h"

void AbstractRobot::EventHandle(const QString &key)
{
        auto  iter = _Handle.find(key);
        if (iter != _Handle.end()){
            auto handle= iter->second;
            if (handle())
                _Handle.erase(iter);
        }
}


void AbstractRobot::setReply(QNetworkReply *reply)
{
      _reply = reply;
}

AbstractRobot::AbstractRobot(QObject *parent) :
    QObject(parent),
    _reply(nullptr),
    _Handle()
{

}

AbstractRobot::~AbstractRobot()
{

}

const std::shared_ptr<QNetworkReply> AbstractRobot::getReply() const
{
    return std::shared_ptr<QNetworkReply>(_reply);
}

