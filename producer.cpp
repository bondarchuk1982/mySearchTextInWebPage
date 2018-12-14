#include "producer.h"

Producer::Producer(const QString& urlStr, const QString& searchText, const QString& urlPattern,
                 QObject *parent)
    : QObject(parent), ScannerInLine(searchText, urlPattern)
    , m_urlStr(urlStr)
{ }

void Producer::run()
{
    if (!openDownloadedUrl()) {
        return;
    }

    auto data = m_inputFile.readAll();

    auto scanResults = searchTargetInLine(QString(data));
    auto newUrls = searchUrlsInLine(QString(data));
    newUrls.removeDuplicates();

    emit searchTextAndUrlsRezult(m_urlStr, newUrls, scanResults.size());

    m_inputFile.close();
}

bool Producer::openDownloadedUrl()
{
    QString dir("downloads/");
    QString fileName = m_urlStr;
    fileName.remove(QRegExp("[-:/.//]"));
    fileName = dir + fileName;

    m_inputFile.setFileName(fileName);
    if (!m_inputFile.open(QIODevice::ReadOnly)) {
        qDebug().noquote()<< "Ошибка! Не возможно прочитать файл " << m_inputFile.fileName();
        return false;
    }
    return true;
}
