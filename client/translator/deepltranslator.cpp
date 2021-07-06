#include <QTimer>
#include <QWebEnginePage>

#include "deepltranslator.h"

QStringList DeepLTranslator::fromLanguages()
{
    return QStringList();
}

QStringList DeepLTranslator::toLanguages()
{
    return QStringList();
}

QString DeepLTranslator::defaultFrom()
{
    return "en";
}

QString DeepLTranslator::defaultTo()
{
    return "zh";
}

void DeepLTranslator::from(const QString &from)
{
    m_from = from;
}

void DeepLTranslator::to(const QString &to)
{
    m_to = to;
}

void DeepLTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    timer->start(intervalStartupStep);
}

QString DeepLTranslator::landingPageUrl()
{
    return "https://www.deepl.com/translator#en/zh/";
}

QString DeepLTranslator::resultJavaScript()
{
    return "document.getElementsByTagName('textarea')[1].value;\n";
}
