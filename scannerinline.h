#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

class ScannerInLine
{
public:
    explicit ScannerInLine(const QString& searchText, const QString& urlPattern);

    /* Метод для поиска совпадений с искомым текстом:
     * line - строка в которой будет поизводится поиск
     */
    QStringList searchTargetInLine(const QString& line);

    /* Метод для поиска адресов web страниц:
     * line - строка в которой будет поизводится поиск
     */
    QStringList searchUrlsInLine(const QString& line);

private:
    /* Регулярка для поиска текста на странице
     */
    QRegularExpression m_targetTextExpr;

    /* Регулярка для поиска адресов web страниц
     */
    QRegularExpression m_urlExpr;

};

