#include "downloader.h"

DownLoader::DownLoader(const QString& url, QObject* parent) :
    QObject(parent)
  , m_urlStr(url)
{ }

void DownLoader::run()
{
    QUrl url(m_urlStr);
    auto request = std::make_unique<QNetworkRequest>(url);
    auto m_uPtrQNAM = std::make_unique<QNetworkAccessManager>();
    auto reply = std::make_unique<QNetworkReply*>((*m_uPtrQNAM).get(*request));

    QEventLoop event;

    connect(*reply, &QNetworkReply::downloadProgress, [this](qint64 part, qint64 max) {
        if (max <= 0 || part <= 0) {
            return;
        }

        emit downloadProgressChanged(part, max, m_urlStr);
    });
    connect(*reply, &QNetworkReply::finished, [this, &event, &reply]() {
        if (!(*reply)->error() && !downloadAborted) {
            saveToFile(reply);
            emit downloadFinished(m_urlStr);
        }

        (*reply)->deleteLater();
        (*reply) = nullptr;
        event.quit();
    });
    connect(*reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [this, &reply](QNetworkReply::NetworkError) {
        emit networkErrorStr(m_urlStr, (*reply)->errorString());
    });
    connect(this, &DownLoader::downloadAbort, [this, &reply](){
        if ((*reply)->isRunning()) {
            (*reply)->abort();
            downloadAborted = true;
        }
    });

    event.exec();
}

void DownLoader::onDownloadAbort()
{
    emit downloadAbort();
}

void DownLoader::saveToFile(const std::unique_ptr<QNetworkReply*>& reply)
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
    outputFile.write((*reply)->readAll());
}
