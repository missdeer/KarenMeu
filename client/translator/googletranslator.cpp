#include <QTimer>
#include <QWebEnginePage>

#include "googletranslator.h"

QStringList GoogleTranslator::fromLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/google.json", "fromLanguages");
}

QStringList GoogleTranslator::toLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/google.json", "toLanguages");
}

QString GoogleTranslator::defaultFrom() const
{
    return TranslatorUtil::String(":/rc/translators/google.json", "defaultFrom");
}

QString GoogleTranslator::defaultTo() const
{
    return TranslatorUtil::String(":/rc/translators/google.json", "defaultTo");
}

void GoogleTranslator::from(const QString &from)
{
    m_from = from;
}

void GoogleTranslator::to(const QString &to)
{
    m_to = to;
}

QString GoogleTranslator::from() const
{
    return TranslatorUtil::String(":/rc/translators/google.json", m_from);
}

QString GoogleTranslator::to() const
{
    return TranslatorUtil::String(":/rc/translators/google.json", m_to);
}

void GoogleTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    timer->start(intervalStartupStep);
}

QString GoogleTranslator::landingPageUrl() const
{
    return QString("https://translate.google.cn/?sl=%1&tl=%2&text=").arg(from(), to());
}

QString GoogleTranslator::resultJavaScript() const
{
    return "document.getElementsByClassName('VIiyi')[0].innerText;\n";
}
