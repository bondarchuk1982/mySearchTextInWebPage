#include "manager.h"

Manager::Manager(QWidget *parent)
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

    this->creatTableWidget();

    QDir cDir = QDir::current();
    QString dir("downloads");
    if (!cDir.mkdir(dir)) {
        qDebug().noquote() << "Ошибка создание каталога!";
    }
}

Manager::~Manager()
{
    QDir cDir = QDir::current();
    QString dir("downloads");
    cDir.cd(dir);
    if (!cDir.removeRecursively()) {
        qDebug().noquote() << "Ошибка удаления каталога!";
    }
}

void Manager::creatTableWidget()
{
    QStringList headers = { "Адрес:", "Статус:", "Прогресс:", "Найдено:", "Описание ошибки:" };

    this->tableWidget->setColumnCount(5); // Указываем число колонок
    this->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    this->tableWidget->setHorizontalHeaderLabels(headers);
    // Растягиваем последнюю колонку на всё доступное пространство
    this->tableWidget->setColumnWidth(2, 250);
    this->tableWidget->horizontalHeader()->setStretchLastSection(true);
    this->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Manager::initNewRowInTableWidget(const QString &name)
{
    this->tableWidget->insertRow(this->tableWidget->rowCount());

    for (quint8 i = 0; i < 5; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem;

        switch (i) {
            case 0: item->setText(name); break;
            case 1: item->setText("Закачка."); break;
            case 2: item->setText("?/?"); break;
            case 3: item->setText("0"); break;
            case 4: item->setText(""); break;
        }

        item->setTextAlignment(Qt::AlignLeft);
        this->tableWidget->setItem(this->tableWidget->rowCount() - 1, i, item);
    }
}

void Manager::onStartBtnClick()
{
    qDebug().noquote() << "StartBtnClick()";
    if (startUrlLEdit->text() != "") {
        downloadPage(startUrlLEdit->text());
    }
}
void Manager::onStopBtnClick()
{
    qDebug().noquote() << "StopBtnClick()";
}
void Manager::onPauseBtnClick()
{
    qDebug().noquote() << "PauseBtnClick()";
}

void Manager::downloadPage(const QString &name)
{
    // downloader will be deleted by global Thread pool
    DownLoader* downloader = new DownLoader(name);

    connect(downloader, &DownLoader::downloadProgressChanged,
            this, &Manager::onDownloadProgressChanged);
    connect(downloader, &DownLoader::downloadFinished,
            this, &Manager::onDownloadFinished);
   // connect(this, &SearchEngine::abort_download, downloader, &DownLoader::)
    initNewRowInTableWidget(name);

    m_threadPoolForDownloads->start(downloader);
}

void Manager::onDownloadProgressChanged(qint64 part, qint64 max, QString url)
{
    qDebug().noquote() << "part: " << part << "max: " << max << "url: " << url << "\n";

    QString str = "";
    max != -1 ? str = QString::number(part) + "/" + QString::number(max)
              : str = QString::number(part) + "/?";

    for (quint16 i = 0; i < this->tableWidget->rowCount(); ++i) {
        auto itm = this->tableWidget->item(i, 0);

        if(itm->text() == url) {
            this->tableWidget->setItem(i, 2, new QTableWidgetItem(str));
        }
    }
    this->tableWidget->resizeColumnsToContents();
    this->tableWidget->horizontalHeader()->setStretchLastSection(true);
}
void Manager::onDownloadFinished(QString url)
{
    qDebug().noquote() << "DownloadFinished: " << url << "\n";

}
