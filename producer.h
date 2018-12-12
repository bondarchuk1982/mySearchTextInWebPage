#pragma once

#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

#include "scannerinline.h"

class Producer : public QObject, public QRunnable, public ScannerInLine
{
    Q_OBJECT
public:
    explicit Producer(const QString& urlStr, const QString& searchText, const QString& urlPattern,
            QObject* parent = nullptr);

    void run() override;

signals:
    void searchTextAndUrlsRezult(const QString& urlStr, const QStringList& newUrls, int count);

private:
    bool openDownloadedUrl();

    QString m_urlStr;

    QFile m_inputFile;
};

