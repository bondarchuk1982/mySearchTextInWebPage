#include "manager.h"

Manager::Manager(QWidget *parent)
    : QWidget(parent)
    , startUrlLbl(new QLabel("Старотовый Url:"))
    , maxUrlsLbl(new QLabel("Максимальное количество сканируемых Url:"))
    , textForSearchLbl(new QLabel("Искомый текст:"))
    , maxThreadsLbl(new QLabel("Максимальное количество потоков:"))
    , startUrlLEdit(new QLineEdit(""))
    , maxUrlsLEdit(new QLineEdit(""))
    , textForSearchLEdit(new QLineEdit(""))
    , maxThreadsLEdit(new QLineEdit(""))
    , startBtn(new QPushButton("Start"))
    , stopBtn(new QPushButton("Stop"))
    , tableWidget(new QTableWidget)
{
    auto vSplitter = new QSplitter(Qt::Vertical);

    vSplitter->addWidget(startUrlLbl);
    vSplitter->addWidget(startUrlLEdit);
    vSplitter->addWidget(maxUrlsLbl);
    vSplitter->addWidget(maxUrlsLEdit);
    vSplitter->addWidget(textForSearchLbl);
    vSplitter->addWidget(textForSearchLEdit);
    vSplitter->addWidget(maxThreadsLbl);
    vSplitter->addWidget(maxThreadsLEdit);

    auto hSplitter = new QSplitter(Qt::Horizontal);

    hSplitter->setMinimumWidth(750);
    hSplitter->addWidget(startBtn);
    hSplitter->addWidget(stopBtn);

    auto vlayo = new QVBoxLayout;
    vlayo->setMargin(5);
    vlayo->addWidget(vSplitter);
    vlayo->addWidget(hSplitter);
    vlayo->addWidget(tableWidget);

    this->setLayout(vlayo);

    connect(startBtn, SIGNAL(clicked()), SLOT(onStartBtnClick()));
    connect(stopBtn,  SIGNAL(clicked()), SLOT(onStopBtnClick()));

    this->creatTableWidget();

    auto cDir = QDir::current();
    QString dir("downloads");
    cDir.mkdir(dir);
    if (!cDir.mkdir(dir)) {
        qDebug().noquote()<< "Ошибка создание каталога!";
    }
}

Manager::~Manager()
{
    if (!m_stopped) {
        onStopBtnClick();
    }

    auto cDir = QDir::current();
    QString dir("downloads");
    cDir.cd(dir);
    if (!cDir.removeRecursively()) {
        qDebug().noquote()<< "Ошибка удаления каталога!";
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
        auto item = new QTableWidgetItem;

        switch (i) {
            case 0: item->setText(name); break;
            case 1: item->setText("Закачка."); break;
            case 2: item->setText("?/?"); break;
            case 3: item->setText("0"); break;
            case 4: item->setText(""); break;
        }

        item->setTextAlignment(Qt::AlignLeft);
        this->tableWidget->setItem(this->tableWidget->rowCount() - 1, i, item);
        this->tableWidget->scrollToBottom();
    }
}

void Manager::onStartBtnClick()
{
    if (startUrlLEdit->text() == ""      || maxUrlsLEdit->text() == "" ||
        textForSearchLEdit->text() == "" || maxThreadsLEdit->text() == "") {
        return;
    }

    m_maxURLCount = maxUrlsLEdit->text().toInt();
    m_targetText  = textForSearchLEdit->text();
    m_maxThreads  = maxThreadsLEdit->text().toInt();

    m_threadPool->setMaxThreadCount(m_maxThreads);
    m_threadPool->setExpiryTimeout(100);

    m_downloadedUrls.reserve(m_maxURLCount);

    downloadPage(startUrlLEdit->text());

    m_stopped = false;
}
void Manager::onStopBtnClick()
{
    emit downloadAbort();

    m_stopped = true;

    m_threadPool->clear();

    m_downloadedUrls.clear();
    m_URLsForScan.clear();
}

void Manager::downloadPage(const QString &name)
{
    // downloader will be deleted by global Thread pool
    auto downloader = new DownLoader(name);

    connect(downloader, &DownLoader::downloadProgressChanged,
            this, &Manager::onDownloadProgressChanged);
    connect(downloader, &DownLoader::downloadFinished,
            this, &Manager::onDownloadFinished);
    connect(downloader, &DownLoader::networkErrorStr,
            this, &Manager::onNetworkErrorCode);
    connect(this, &Manager::downloadAbort,
            downloader, &DownLoader::onDownloadAbort);

    initNewRowInTableWidget(name);

    m_threadPool->start(downloader);
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
            part != max ? str = QString::number(part*100/max) + "%"
                        : str = "100%";

            this->tableWidget->setItem(i, 2, new QTableWidgetItem(str));
            break;
        }
    }
}
void Manager::onDownloadFinished(const QString& url)
{
    if (m_stopped) {
        return;
    }

    m_downloadedUrls.push_back(url);

    auto producer = new Producer(url, m_targetText, urlPattern);

    connect(producer, &Producer::searchTextAndUrlsRezult,
            this, &Manager::onSearchTextAndUrlsFinished);

    // producer will be deleted by QThreadPool
    m_threadPool->start(producer);
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

    searchTextManager(newUrls);
}
void Manager::searchTextManager(const QStringList& newUrls)
{
    int uRLsCount = m_URLsForScan.size() + m_downloadedUrls.size() + m_threadPool->activeThreadCount();

    if (uRLsCount < m_maxURLCount && !newUrls.empty()) {
        for (const auto& str : newUrls) {
            if (uRLsCount == m_maxURLCount) {
                break;
            }

            if (str == "" ||
                m_URLsForScan.end()    != std::find(m_URLsForScan.begin(), m_URLsForScan.end(), str) ||
                m_downloadedUrls.end() != std::find(m_downloadedUrls.begin(), m_downloadedUrls.end(), str)) {
                continue;
            }

            ++uRLsCount;
            m_URLsForScan.push_back(str);
        }
    }

    while (m_threadPool->activeThreadCount() < m_maxThreads) {
        if (m_URLsForScan.empty()) {
            return;
        }

        downloadPage(m_URLsForScan.front());
        m_URLsForScan.pop_front();
    }
}

void Manager::onNetworkErrorCode(const QString &url, const QString &str)
{
    for (int i = this->tableWidget->rowCount() - 1; i >= 0; --i) {
        if(this->tableWidget->item(i, 0)->text() == url) {
            this->tableWidget->setItem(i, 1, new QTableWidgetItem("Ошибка."));
            this->tableWidget->setItem(i, 2, new QTableWidgetItem("Error!"));
            this->tableWidget->setItem(i, 4, new QTableWidgetItem(str));

            m_downloadedUrls.push_back(url);

            QStringList nullList;
            searchTextManager(nullList);

            break;
        }
    }
}
