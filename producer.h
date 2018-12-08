#ifndef PRODUCER_H
#define PRODUCER_H

#include <QWidget>
#include <QtWidgets>

class Producer : public QWidget
{
    Q_OBJECT

public:
    Producer(QWidget *parent = nullptr);
    ~Producer() = default;

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
public slots:
    void onStartBtnClick();
    void onStopBtnClick();
    void onPauseBtnClick();
};

#endif // PRODUCER_H
