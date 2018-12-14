#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QRunnable>
#include <QNetworkReply>
#include <QEventLoop>
#include <QThread>
#include <QFile>
#include <QFileInfo>


class DownLoader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    /* Конструктор принимает параметр url
     * url - адрес сайта, который будем закачивать
     */
    explicit DownLoader(const QString& url, QObject* parent = nullptr);

    /* Перегруженный метод для запуска в потоке
     */
    void run() override;

signals:
    /* Сигнал прогресса закачки
     * part - текущее количество закачаных байт
     * max - максимальное количество байт, которое будет закачано
     * url - адрес сайта, который закачивается
     */
    void downloadProgressChanged(qint64 part, qint64 max, const QString& url);

    /* Сигнал о завершении закачки
     * url - адрес сайта, который закачался
     */
    void downloadFinished(const QString& url);

    /* Сигнал об ошибке закачки
     * url - адрес сайта, который выдал ошибку
     * errorStr - описание ошибки
     */
    void networkErrorStr(const QString& url, const QString& errorStr);

    /* Сигнал прервать все закачки
     */
    void downloadAbort();

public slots:
    /* Слот для приёма внешнего сигнала прервать все закачки
     */
    void onDownloadAbort();

private:
    /* Метод для сохранения закачаного сайта в файл. В качестве
     * параметра принимает ссылку на уникальный указатель хранящий
     * в себе указатель на объект QNetworkReply
     */
    void saveToFile(const std::unique_ptr<QNetworkReply*>& reply);

    /* Флаг прервать все закачки
     */
    bool downloadAborted = false;

    /* Внутрення строка хранящая в себе адрес сайта
     */
    QString m_urlStr;
};
