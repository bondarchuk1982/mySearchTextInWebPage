#include "producer.h"

Producer::Producer(QWidget *parent)
    : QWidget(parent)
    , startUrlLbl(new QLabel("Старотовый Url:"))
    , maxUrlsLbl(new QLabel("Максимальное количество сканируемых Url:"))
    , textForSearchLbl(new QLabel("Искомый текст:"))
    , threadsPoolLbl(new QLabel("Максимальное количество потоков:"))
    , startUrlLEdit(new QLineEdit)
    , maxUrlsLEdit(new QLineEdit)
    , textForSearchLEdit(new QLineEdit)
    , threadsPoolLEdit(new QLineEdit)
    , startBtn(new QPushButton("Start"))
    , stopBtn(new QPushButton("Stop"))
    , pauseBtn(new QPushButton("Pause"))
    , tableWidget(new QTableWidget)
{
    QSplitter* vSplitter = new QSplitter(Qt::Vertical);

    vSplitter->addWidget(startUrlLbl);
    vSplitter->addWidget(startUrlLEdit);
    vSplitter->addWidget(maxUrlsLbl);
    vSplitter->addWidget(maxUrlsLEdit);
    vSplitter->addWidget(textForSearchLbl);
    vSplitter->addWidget(textForSearchLEdit);
    vSplitter->addWidget(threadsPoolLbl);
    vSplitter->addWidget(threadsPoolLEdit);

    QSplitter* hSplitter = new QSplitter(Qt::Horizontal);

    hSplitter->setMinimumWidth(750);
    hSplitter->addWidget(startBtn);
    hSplitter->addWidget(stopBtn);
    hSplitter->addWidget(pauseBtn);

    QVBoxLayout* vlayo = new QVBoxLayout;
    vlayo->setMargin(5);
    vlayo->addWidget(vSplitter);
    vlayo->addWidget(hSplitter);
    vlayo->addWidget(tableWidget);

    this->setLayout(vlayo);

    connect(startBtn, SIGNAL(clicked()), SLOT(onStartBtnClick()));
    connect(stopBtn,  SIGNAL(clicked()), SLOT(onStopBtnClick()));
    connect(pauseBtn, SIGNAL(clicked()), SLOT(onPauseBtnClick()));
}

void Producer::onStartBtnClick()
{
    qDebug().noquote() << "StartBtnClick()";
}

void Producer::onStopBtnClick()
{
    qDebug().noquote() << "StopBtnClick()";
}

void Producer::onPauseBtnClick()
{
    qDebug().noquote() << "PauseBtnClick()";
}
