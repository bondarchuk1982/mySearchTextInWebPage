#include "manager.h"

Manager::Manager(QWidget *parent)
    : QWidget(parent)
    , startUrlLbl(new QLabel("Старотовый Url:"))
    , maxUrlsLbl(new QLabel("Максимальное количество сканируемых Url:"))
    , textForSearchLbl(new QLabel("Искомый текст:"))
    , threadsPoolLbl(new QLabel("Максимальное количество потоков:"))
    , startUrlLEdit(new QLineEdit("https://software.intel.com/ru-ru/articles/producer-consumer"))
    , maxUrlsLEdit(new QLineEdit("50"))
    , textForSearchLEdit(new QLineEdit("producer"))
    , threadsPoolLEdit(new QLineEdit("8"))
    , startBtn(new QPushButton("Start"))
    , stopBtn(new QPushButton("Stop"))
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

    QVBoxLayout* vlayo = new QVBoxLayout;
    vlayo->setMargin(5);
    vlayo->addWidget(vSplitter);
    vlayo->addWidget(hSplitter);
    vlayo->addWidget(tableWidget);

    this->setLayout(vlayo);

    connect(startBtn, SIGNAL(clicked()), SLOT(onStartBtnClick()));
    connect(stopBtn,  SIGNAL(clicked()), SLOT(onStopBtnClick()));

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

    if (!m_stopped) {
        onStopBtnClick();
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
    if (startUrlLEdit->text() == "" || maxUrlsLEdit->text() == "" ||
        textForSearchLEdit->text() == "" || threadsPoolLEdit->text() == "") {
        return;
    }

    m_URLToScan             = startUrlLEdit->text();
    m_maxURLCount           = maxUrlsLEdit->text().toInt();
    m_targetText            = textForSearchLEdit->text();
    m_maxThreadForDownloads = threadsPoolLEdit->text().toInt();

    m_threadPoolForDownloads->setMaxThreadCount(m_maxThreadForDownloads);
    m_threadPoolForDownloads->setExpiryTimeout(100);

    downloadPage(m_URLToScan);

    m_stopped = false;
}
void Manager::onStopBtnClick()
{
    m_stopped = true;

    m_threadPoolForDownloads->clear();
    m_threadPoolForLocalSearch.clear();
    m_threadPoolForLocalSearch.deleteLater();

    m_downloadedUrls.clear();
    m_URLsForScan.clear();

    m_totalDownloadUrls = 0;
}

void Manager::downloadPage(const QString &name)
{
    // downloader will be deleted by global Thread pool
    DownLoader* downloader = new DownLoader(name);

    connect(downloader, &DownLoader::downloadProgressChanged,
            this, &Manager::onDownloadProgressChanged);
    connect(downloader, &DownLoader::downloadFinished,
            this, &Manager::onDownloadFinished);
    connect(downloader, &DownLoader::networkErrorStr,
            this, &Manager::onNetworkErrorCode);
    initNewRowInTableWidget(name);

    m_threadPoolForDownloads->start(downloader);

    ++m_totalDownloadUrls;
}
void Manager::onDownloadProgressChanged(qint64 part, qint64 max, const QString& url)
{
    if (m_stopped) {
        return;
    }

    for (int i = this->tableWidget->rowCount() - 1; i >= 0; --i) {
        if(this->tableWidget->item(i, 0)->text() == url) {
            if (this->tableWidget->item(i, 4)->text() != "") {
                return;
            }

            QString str = "";
            max != -1 ? str = QString::number(part) + "/" + QString::number(max)
                      : str = QString::number(part) + "/?";

            this->tableWidget->setItem(i, 2, new QTableWidgetItem(str));
            break;
        }
    }
    this->tableWidget->horizontalHeader()->setStretchLastSection(true);
}
void Manager::onDownloadFinished(const QString& url)
{
    if (m_stopped) {
        return;
    }

    m_downloadedUrls.insert(url);
    --m_totalDownloadUrls;

    for (int i = this->tableWidget->rowCount() - 1; i >= 0; --i) {
        if (this->tableWidget->item(i, 0)->text() == url) {
            if (this->tableWidget->item(i, 4)->text() != "") {
                return;
            }

            this->tableWidget->setItem(i, 2, new QTableWidgetItem("100%"));
            break;
        }
    }

    Producer* producer = new Producer(url, m_targetText, urlPattern);

    connect(producer, &Producer::searchTextAndUrlsRezult,
            this, &Manager::onSearchTextAndUrlsFinished);

    // producer will be deleted by QThreadPool
    m_threadPoolForLocalSearch.start(producer);
}

void Manager::onSearchTextAndUrlsFinished(const QString& url, const QStringList& newUrls, int count)
{
    if (m_stopped) {
        return;
    }

    for (int i = this->tableWidget->rowCount() - 1; i >= 0; --i) {
        if(this->tableWidget->item(i, 0)->text() == url) {
            this->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(count)));

            count > 0 ? this->tableWidget->setItem(i, 1, new QTableWidgetItem("Найдено")):
                        this->tableWidget->setItem(i, 1, new QTableWidgetItem("Не найдено"));

            break;
        }
    }
    this->tableWidget->horizontalHeader()->setStretchLastSection(true);

    searchTextManager(newUrls);
}
void Manager::searchTextManager(const QStringList& newUrls)
{
    for (const auto& str : newUrls) {
        if (m_URLsForScan.size() == m_maxURLCount) {
            break;
        }

        if (m_URLsForScan.end() != std::find(m_URLsForScan.begin(), m_URLsForScan.end(), str)) {
            continue;
        }

        m_URLsForScan.push_back(str);
    }

    while (m_totalDownloadUrls < m_maxThreadForDownloads) {
        if (m_URLsForScan.empty()) {
            return;
        }

        if (m_URLsForScan.front() == "" ||
            m_downloadedUrls.end() != m_downloadedUrls.find(m_URLsForScan.front())) {
            m_URLsForScan.pop_front();
            continue;
        }

        downloadPage(m_URLsForScan.front());
        m_URLsForScan.pop_front();
    }
}

void Manager::onNetworkErrorCode(const QString& url, const QString& str)
{
    for (int i = this->tableWidget->rowCount() - 1; i >= 0; --i) {
        if(this->tableWidget->item(i, 0)->text() == url) {
            this->tableWidget->setItem(i, 1, new QTableWidgetItem("Ошибка."));
            this->tableWidget->setItem(i, 2, new QTableWidgetItem("Error!"));
            this->tableWidget->setItem(i, 4, new QTableWidgetItem(str));
            break;
        }
    }
}
