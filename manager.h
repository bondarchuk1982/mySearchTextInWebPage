#ifndef PRODUCER_H
#define PRODUCER_H

#include <QWidget>
#include <QtWidgets>
#include <QDir>
#include <QThreadPool>
#include <QDebug>
#include <QStringList>

#include "downloader.h"
#include "producer.h"

class Manager : public QWidget
{
    Q_OBJECT

public:
    Manager(QWidget *parent = nullptr);
    ~Manager();

protected:
    QLabel* startUrlLbl;
    QLabel* maxUrlsLbl;
    QLabel* textForSearchLbl;
    QLabel* maxThreadsLbl;

    QLineEdit* startUrlLEdit;
    QLineEdit* maxUrlsLEdit;
    QLineEdit* textForSearchLEdit;
    QLineEdit* maxThreadsLEdit;

    QPushButton* startBtn;
    QPushButton* stopBtn;

    QTableWidget* tableWidget;

    void creatTableWidget();
    void initNewRowInTableWidget(const QString& name);

    void downloadPage(const QString& name);

public slots:
    void onStartBtnClick();
    void onStopBtnClick();

    void onDownloadProgressChanged(qint64 part, qint64 max, const QString& url);
    void onDownloadFinished(const QString& url);
    void onSearchTextAndUrlsFinished(const QString& urlStr, const QStringList& newUrls, int count);
    void onNetworkErrorCode(const QString& url, const QString& str);

signals:
    void downloadAbort();

private:
    void searchTextManager(const QStringList& newUrls);

    QThreadPool* m_threadPool = QThreadPool::globalInstance();

    QVector<QString> m_downloadedUrls;
    QQueue<QString> m_URLsForScan;

    QString m_targetText;

    int m_maxURLCount = -1;
    int m_maxThreads = -1;

    bool m_stopped = false;

    const QString urlPattern =
          QString("(http|ftp|https)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?");

};
#endif // PRODUCER_H
