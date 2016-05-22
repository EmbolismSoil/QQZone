#ifndef TULINGROBOT_H
#define TULINGROBOT_H

#include <abstractrobot.h>
#include <QObject>
#include <qqhttp.h>
#include <memory>
#include <myhttp.h>
#include <QNetworkReply>

class TulingRobot : public AbstractRobot
{
private:
    std::shared_ptr<MyHttp>  _ptrHttp;
    QString _key;
public:
    TulingRobot(const QString &key, QObject *parent = 0);
    virtual ~TulingRobot();
    virtual bool request(const QString &, QString const &, const CallBack &, Method m = GET) override;
    QNetworkReply* getReply(){
        return  _ptrHttp->getReply();
    }

protected slots:
    virtual void readyReply(QNetworkReply *) override;
};

#endif // TULINGROBOT_H
