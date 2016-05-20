#ifndef TULINGROBOT_H
#define TULINGROBOT_H

#include <abstractrobot.h>
#include <QObject>
#include <qqhttp.h>
#include <memory>

class TulingRobot : public AbstractRobot
{
private:
    std::shared_ptr<QQHttp>  _ptrHttp;
    QString _TulingURL;

public:
    TulingRobot(const QString &key, const QString &id, QObject *parent = 0);
    virtual ~TulingRobot();
    virtual bool request(const QString &, const CallBack &, Method m = GET) override;

protected slots:
    virtual void readyReply(QNetworkReply *) override;
};

#endif // TULINGROBOT_H
