#ifndef PRODUCER_H
#define PRODUCER_H

#include <QWidget>
#include <QtWidgets>
#include <QDir>
#include <QThreadPool>
#include <QDebug>
#include <QStringList>

#include "downloader.h"

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
    QLabel* threadsPoolLbl;

    QLineEdit* startUrlLEdit;
    QLineEdit* maxUrlsLEdit;
    QLineEdit* textForSearchLEdit;
    QLineEdit* threadsPoolLEdit;

    QPushButton* startBtn;
    QPushButton* stopBtn;
    QPushButton* pauseBtn;

    QTableWidget* tableWidget;
    void creatTableWidget();
    void initNewRowInTableWidget(const QString& name);

    void downloadPage(const QString& name);

public slots:
    void onStartBtnClick();
    void onStopBtnClick();
    void onPauseBtnClick();

    void onDownloadProgressChanged(qint64 part, qint64 max, QString url);
    void onDownloadFinished(QString url);
private:
    QThreadPool* m_threadPoolForDownloads = QThreadPool::globalInstance();
};
#endif // PRODUCER_H
