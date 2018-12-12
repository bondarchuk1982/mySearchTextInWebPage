#include "downloader.h"
#include <QNetworkReply>

#include <QEventLoop>
#include <QThread>
#include <QFile>
#include <QFileInfo>

DownLoader::DownLoader(const QString& url, QObject* parent) :
    QObject(parent)
  , m_urlStr(url)
{ }

void DownLoader::run()
{
    QUrl url(m_urlStr);
    QNetworkRequest request(url);
    auto m_uPtrQNAM = std::make_unique<QNetworkAccessManager>();
    auto reply = m_uPtrQNAM.get()->get(request);
    QEventLoop event;

    connect(reply, &QNetworkReply::downloadProgress, [this](qint64 part, qint64 max) {
        emit downloadProgressChanged(part, max, m_urlStr);
    });
    connect(reply, &QNetworkReply::finished, [this, &event, &reply]() {
        saveToFile(reply);

        emit downloadFinished(m_urlStr);

        reply->deleteLater();
        event.quit();
    });
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                   SLOT(onNetworkErrorCode(QNetworkReply::NetworkError)));

    event.exec();
}

void DownLoader::onNetworkErrorCode(QNetworkReply::NetworkError error)
{
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            emit networkErrorStr(m_urlStr, "удалённый сервер отклонил соединение (сервер не принимает соединения)"); break;
        case QNetworkReply::RemoteHostClosedError:
            emit networkErrorStr(m_urlStr, "удалённый сервер принудительно закрыл соединение до того, как весь ответ был получен и обработан"); break;
        case QNetworkReply::HostNotFoundError:
            emit networkErrorStr(m_urlStr, "имя удалённого сервера не было найдено (неверное имя сервера)"); break;
        case QNetworkReply::TimeoutError:
            emit networkErrorStr(m_urlStr, "истекло время соединения с удалённым сервером"); break;
        case QNetworkReply::OperationCanceledError:
            emit networkErrorStr(m_urlStr, "операция была отменена с помощью вызовов abort() или close() перед тем как она была завершена."); break;
        case QNetworkReply::TemporaryNetworkFailureError:
            emit networkErrorStr(m_urlStr, "соединение было разорвано из-за отключения от сети, однако система установила автоматическое соединение с другой точкой доступа. Запрос будет отправлен заново и обработан как только будет восстановлено соединение."); break;
        case QNetworkReply::ProxyConnectionRefusedError:
            emit networkErrorStr(m_urlStr, "соединение с прокси-сервером было отклонено (прокси-сервер не принимает запросы)"); break;
        case QNetworkReply::ProxyConnectionClosedError:
            emit networkErrorStr(m_urlStr, "прокси-сервер принудительно закрыл соединение до того, как весь ответ был получен и обработан"); break;
        case QNetworkReply::ProxyNotFoundError:
            emit networkErrorStr(m_urlStr, "имя прокси-сервера не было найдено (неверное имя прокси-сервера)"); break;
        case QNetworkReply::ProxyTimeoutError:
            emit networkErrorStr(m_urlStr, "время соединения с прокси-сервером истекло или прокси-сервер не ответил вовремя после отправления запроса"); break;
        case QNetworkReply::ProxyAuthenticationRequiredError:
            emit networkErrorStr(m_urlStr, "Прокси-сервер требует аутентификацию чтобы выполнить запрос, но не принял текущую аутентификационную информацию (если такая есть)"); break;
        case QNetworkReply::ContentAccessDenied:
            emit networkErrorStr(m_urlStr, "доступ к удалённому содержимому был запрещен (аналогично ошибке HTTP 401)"); break;
        case QNetworkReply::ContentOperationNotPermittedError:
            emit networkErrorStr(m_urlStr, "операция, запрошенная на удалённом содержимом, не позволяется"); break;
        case QNetworkReply::ContentNotFoundError:
            emit networkErrorStr(m_urlStr, "удалённое содержимое не было найдено на сервере (аналогично ошибке HTTP 404)"); break;
        case QNetworkReply::AuthenticationRequiredError:
            emit networkErrorStr(m_urlStr, "удалённый сервер требует аутентификацию чтобы обработать содержимое, но не принял текущую аутентификационную информацию (если такая есть)"); break;
        case QNetworkReply::ContentReSendError:
            emit networkErrorStr(m_urlStr, "запрос необходимо отправить снова, но это завершится неудачей, например, поскольку отправляемые данные не могут быть прочитаны во второй раз."); break;
        case QNetworkReply::ProtocolUnknownError:
            emit networkErrorStr(m_urlStr, "API сетевого доступа не смогло обработать запрос из-за того, что протокол не известен"); break;
        case QNetworkReply::ProtocolInvalidOperationError:
            emit networkErrorStr(m_urlStr, "запрошенная операция недопустима для этого протокола"); break;
        case QNetworkReply::UnknownNetworkError:
            emit networkErrorStr(m_urlStr, "была обнаружена неизвестная ошибка, связанная с сетью"); break;
        case QNetworkReply::UnknownProxyError:
            emit networkErrorStr(m_urlStr, "была обнаружена неизвестная ошибка, связанная с прокси-сервером"); break;
        case QNetworkReply::UnknownContentError:
            emit networkErrorStr(m_urlStr, "была обнаружена неизвестная ошибка, связанная с удалённым содержимым"); break;
        case QNetworkReply::ProtocolFailure:
            emit networkErrorStr(m_urlStr, "был обнаружен сбой в протоколе (ошибка обработки, недействительный или неожиданный ответ и т.д.)"); break;
        default: emit networkErrorStr(m_urlStr, "Неизвестная ошибка"); break;
    }
}

void DownLoader::saveToFile(QNetworkReply *reply)
{
    QString dir("downloads/");

    QString fileName = m_urlStr;
    fileName.remove(QRegExp("[-:/.//]"));
    fileName = dir + fileName;

    QFile outputFile(fileName);
    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug().noquote()<< "Ошибка записи в файл "<< outputFile.fileName();
        return;
    }
    outputFile.write(reply->readAll());
}
