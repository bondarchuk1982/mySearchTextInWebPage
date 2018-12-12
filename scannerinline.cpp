#include "scannerinline.h"
#include <QRegularExpressionMatchIterator>
#include <QVector>

ScannerInLine::ScannerInLine(const QString& searchText, const QString& urlPattern)
  : m_targetTextExpr(searchText)
  , m_urlExpr(urlPattern,
              QRegularExpression::ExtendedPatternSyntaxOption |
              QRegularExpression::MultilineOption |
              QRegularExpression::DotMatchesEverythingOption |
              QRegularExpression::CaseInsensitiveOption)
{ }

QStringList ScannerInLine::searchTargetInLine(const QString& line)
{
    QRegularExpressionMatchIterator it = m_targetTextExpr.globalMatch(line);
    QStringList foundPositions;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        foundPositions.append(QString::number(match.capturedStart() + 1));
    }

    return foundPositions;
}

QStringList ScannerInLine::searchUrlsInLine(const QString& line)
{
    QRegularExpressionMatchIterator it = m_urlExpr.globalMatch(line);
    QStringList URLs;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        URLs.append(match.captured());
    }

    return URLs;
}
