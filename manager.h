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
#include "progressbardelegate.h"

class Manager : public QWidget
{
    Q_OBJECT

public:
    Manager(QWidget *parent = nullptr);
    ~Manager();

protected:
    /* Набор QLabel для отображения в окне
     */
    QLabel* startUrlLbl;
    QLabel* maxUrlsLbl;
    QLabel* textForSearchLbl;
    QLabel* maxThreadsLbl;

    /* Набор QLineEdit для отображения в окне и приёма входных параметров
     */
    QLineEdit* startUrlLEdit;
    QLineEdit* maxUrlsLEdit;
    QLineEdit* textForSearchLEdit;
    QLineEdit* maxThreadsLEdit;

    /* Кнопка для старта выполнения задач
     */
    QPushButton* startBtn;

    /* Кнопка для остановки выполнения задач
     */
    QPushButton* stopBtn;

    /* Таблица для отображения прогресса выполнения задач
     */
    QTableWidget* tableWidget;

    /* Инициализируем и задаём начальные парметны таблицы
     */
    void creatTableWidget();

    /* Добавляем новую строку в таблицу под новую задачу:
     * name - адрес web сайта для новой задачи
    */
    void initNewRowInTableWidget(const QString& name);

    /* Метод добавляет задачу по закачке web сайта:
     * name - адрес web сайта для закачки
     */
    void downloadPage(const QString& name);

public slots:
    /* Слот для реакции на нажатие кнопки Start
     */
    void onStartBtnClick();

    /* Слот для реакции на нажатие кнопки Stop
     */
    void onStopBtnClick();

    /* Слот для отображения текущего состояния по закачке web страницы:
     * part - текущее количество закачаных байт
     * max - максимальное количество байт, которое будет закачано
     * url - адрес сайта, который закачивается
     */
    void onDownloadProgressChanged(qint64 part, qint64 max, const QString& url);

    /* Слот для обработки сигнала о завершении закачки
     * url - адрес сайта, который закачался
     */
    void onDownloadFinished(const QString& url);

    /* Слот для обработки сигнала о завершении поиска на web странице
     * urlStr - web страница на которой осуществлялся поиск
     * newUrls - список найденных адресов web страниц
     * count - количество найденных совпадений с искомым текстом
     */
    void onSearchTextAndUrlsFinished(const QString& urlStr, const QStringList& newUrls, int count);

    /* Слот для обработки сигнал об ошибке закачки
     * url - адрес сайта, который выдал ошибку
     * errorStr - описание ошибки
     */
    void onNetworkErrorCode(const QString& url, const QString& str);

signals:
    /* Сигнал прервать все операции в потоках
     */
    void downloadAbort();

private:
    /* Метод менеджер задач. Принимает в качестве парметра список новых web страниц (newUrls).
     * Ставит их в очередь на закачку и в случае освобождения потока закачки запускает
     * новый поток с новой задачей по закачке web страницы стоящей первой в очереди
     */
    void taskManager(const QStringList& newUrls);

    /* Указатель на глобальный пулл потоков
     */
    QThreadPool* m_threadPool = QThreadPool::globalInstance();

    /* Коллекция уже закачанных web страниц
     */
    QVector<QString> m_downloadedUrls;

    /* Коллекция web страниц в очереди
     */
    QQueue<QString> m_URLsForScan;

    /* Внутренний объект для хранения искомого на web страницах текста
     */
    QString m_targetText;

    /* Внутренний объект для хранения максимального количества для обработки web страниц
     */
    int m_maxURLCount = -1;

    /* Внутренний объект для хранения максимального количества потоков для закачки или поиска по web страницам
     */
    int m_maxThreads = -1;

    /* Флаг нажания кнопки Stop
     */
    bool m_stopped = false;

    /* Строка патерн для регулярки по поиску web адресов на закачаных web станицах
     */
    const QString urlPattern =
          QString("(http|ftp|https)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?");

};
#endif // PRODUCER_H
