#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QRunnable>

class DownLoader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit DownLoader(const QString& url, QObject* parent = nullptr);

    void run() override;

signals:
    void downloadProgressChanged(qint64 part, qint64 max, QString url);
    void downloadFinished(QString url);

private:
    void saveToFile(QNetworkReply *reply);

private:
    QString m_urlStr;
};