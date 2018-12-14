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
    /* Конструктор принимает три параметра:
     * urlStr - строка с адресом сайта, который был загружен
     * searchText - строка с текстом, который необходимо найти на сайте
     * urlPattern - паттерн для регулярного выражения по которому будем искать новые адреса сайтов
     */
    explicit Producer(const QString& urlStr, const QString& searchText, const QString& urlPattern,
            QObject* parent = nullptr);

    /* Перегруженный метод для запуска в потоке
     */
    void run() override;

signals:
    /* Сигнал о завершении поиска на web странице
     * urlStr - web страница на которой осуществлялся поиск
     * newUrls - список найденных адресов web страниц
     * count - количество найденных совпадений с искомым текстом
     */
    void searchTextAndUrlsRezult(const QString& urlStr, const QStringList& newUrls, int count);

private:
    /* Метод открывает файл с закачаной web страницей
    */
    bool openDownloadedUrl();

    /* Внутрення строка хранящая в себе адрес сайта
     */
    QString m_urlStr;

    /* Внутренний объек файла хранящая в себе закачанные данные с web страницы
     */
    QFile m_inputFile;
};

