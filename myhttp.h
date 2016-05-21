#ifndef MYHTTP_H
#define MYHTTP_H

#include <QObject>
#include <qqhttp.h>
#include <functional>
#include <vector>
#include <QTimer>

class MyHttp : public QObject
{
    Q_OBJECT
public:
    using CallBack = std::function<bool()>;
    typedef enum{POST = 0, GET, HEAD}Method;

private:
    struct __request{
        QNetworkRequest req;
        MyHttp::CallBack  cb;
        MyHttp::Method  method;
        QByteArray  buf;
    };
    std::shared_ptr<QQHttp> _Http;
    QNetworkReply* _reply;
    std::map<QString, CallBack > _Handles;
    std::shared_ptr<std::list<__request> > _requests;
    std::shared_ptr<QTimer> _timer;

    //std::shared_ptr<QByteArray> _httpBuf;

    void EventHandle(const QString &key);
    bool QRCodeHandler();
    void setReply(QNetworkReply *);

public:
    explicit MyHttp(QObject *parent = 0);
    bool request(const QString &url, const CallBack &,
                    Method method = GET, QByteArray const *buf = nullptr);
    bool request(const QNetworkRequest &req, const CallBack &,
                    Method method = GET, QByteArray const *buf = nullptr);
    QNetworkReply* getReply();
signals:

public slots:
    void onHttpFinished(QNetworkReply *);
    bool doRequest();
    void setPollTime(unsigned int);
};

#endif // MYHTTP_H
