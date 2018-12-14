#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

class ScannerInLine
{
public:
    explicit ScannerInLine(const QString& searchText, const QString& urlPattern);

    QStringList searchTargetInLine(const QString& line);
    QStringList searchUrlsInLine(const QString& line);

private:
    QRegularExpression m_targetTextExpr;
    QRegularExpression m_urlExpr;

};

