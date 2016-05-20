#ifndef ABSTRACTROBOT_H
#define ABSTRACTROBOT_H

#include <QObject>
#include <functional>
#include <memory>
#include <vector>
#include <QNetworkReply>

class AbstractRobot : public QObject
{
    Q_OBJECT
public:
    //如果返回true则表示调用后删除，否则不删除
    typedef enum{POST = 0, GET, HEAD}Method;
    typedef std::function<bool()> CallBack;
    QNetworkReply *_reply;

protected:
    std::map<QString, CallBack>  _Handle;
    void EventHandle(const QString &key);
    void setReply(QNetworkReply *reply);

public:
    explicit AbstractRobot(QObject *parent = 0);
    virtual ~AbstractRobot();
    virtual bool request(const QString &, QString const &, const CallBack &, Method m = GET) = 0;
    virtual const std::shared_ptr<QNetworkReply> getReply()  const;
signals:

protected slots:
    virtual void readyReply(QNetworkReply *)  = 0;
};

#endif // ABSTRACTROBOT_H
