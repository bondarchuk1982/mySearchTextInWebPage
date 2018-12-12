#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QRunnable>
#include <QNetworkReply>

class DownLoader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit DownLoader(const QString& url, QObject* parent = nullptr);

    void run() override;

signals:
    void downloadProgressChanged(qint64 part, qint64 max, const QString& url);
    void downloadFinished(const QString& url);
    void networkErrorStr(const QString& url, const QString& errorStr);

public slots:
    void onNetworkErrorCode(QNetworkReply::NetworkError error);

private:
    void saveToFile(QNetworkReply *reply);

private:
    QString m_urlStr;
};
