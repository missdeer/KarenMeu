#include <QTimer>
#include <QWebEnginePage>

#include "googletranslator.h"

QStringList GoogleTranslator::fromLanguages()
{
    return QStringList();
}

QStringList GoogleTranslator::toLanguages()
{
    return QStringList();
}

QString GoogleTranslator::defaultFrom()
{
    return "en";
}

QString GoogleTranslator::defaultTo()
{
    return "zh";
}

void GoogleTranslator::from(const QString &from)
{
    m_from = from;
}

void GoogleTranslator::to(const QString &to)
{
    m_to = to;
}
void GoogleTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    timer->start(intervalStartupStep);
}

QString GoogleTranslator::landingPageUrl()
{
    return "https://translate.google.com/?sl=auto&tl=zh-CN&text=";
}

QString GoogleTranslator::resultJavaScript()
{
    return "document.getElementsByClassName('VIiyi')[0].innerText;\n";
}
