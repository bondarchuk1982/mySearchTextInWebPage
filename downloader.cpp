#include "downloader.h"
#include <QNetworkReply>

#include <QEventLoop>
#include <QThread>
#include <QFile>
#include <QFileInfo>

DownLoader::DownLoader(const QString& url, QObject* parent) :
    QObject(parent)
  , m_urlStr(url)
{ }

void DownLoader::run()
{
    QUrl url(m_urlStr);
    QNetworkRequest request(url);
    auto m_uPtrQNAM = std::make_unique<QNetworkAccessManager>();
    auto reply = m_uPtrQNAM.get()->get(request);
    QEventLoop event;

    connect(reply, &QNetworkReply::downloadProgress, [this](qint64 part, qint64 max) {
        emit downloadProgressChanged(part, max, m_urlStr);
    });
    connect(reply, &QNetworkReply::finished, [this, &event, &reply]() {
        saveToFile(reply);

        emit downloadFinished(m_urlStr);

        reply->deleteLater();
        event.quit();
    });

    event.exec();
}

void DownLoader::saveToFile(QNetworkReply *reply)
{
    QString dir("downloads/");

    QString fileName = m_urlStr;
    fileName.remove(QRegExp("[-:/.//]"));
    fileName = dir + fileName;

    QFile outputFile(fileName);
    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug().noquote()<< "Ошибка записи в файл "<< outputFile.fileName();
        return;
    }
    outputFile.write(reply->readAll());
}
