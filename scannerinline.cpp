#include "scannerinline.h"

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
    auto it = m_targetTextExpr.globalMatch(line);
    QStringList foundPositions;

    while (it.hasNext()) {
        auto match = it.next();
        foundPositions.append(QString::number(match.capturedStart() + 1));
    }

    return foundPositions;
}

QStringList ScannerInLine::searchUrlsInLine(const QString& line)
{
    auto it = m_urlExpr.globalMatch(line);
    QStringList URLs;

    while (it.hasNext()) {
        auto match = it.next();
        URLs.append(match.captured());
    }

    return URLs;
}
