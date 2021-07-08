#include <QTimer>
#include <QWebEnginePage>

#include "deepltranslator.h"

QStringList DeepLTranslator::fromLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/deepl.json", "fromLanguages");
}

QStringList DeepLTranslator::toLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/deepl.json", "toLanguages");
}

QString DeepLTranslator::defaultFrom() const
{
    return TranslatorUtil::String(":/rc/translators/deepl.json", "defaultFrom");
}

QString DeepLTranslator::defaultTo() const
{
    return TranslatorUtil::String(":/rc/translators/deepl.json", "defaultTo");
}

void DeepLTranslator::from(const QString &from)
{
    m_from = from;
}

void DeepLTranslator::to(const QString &to)
{
    m_to = to;
}

QString DeepLTranslator::from() const
{
    return TranslatorUtil::String(":/rc/translators/deepl.json", m_from);
}

QString DeepLTranslator::to() const
{
    return TranslatorUtil::String(":/rc/translators/deepl.json", m_to);
}

void DeepLTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    timer->start(intervalStartupStep);
}

QString DeepLTranslator::landingPageUrl() const
{
    return QString("https://www.deepl.com/translator#%1/%2/").arg(from(), to());
}

QString DeepLTranslator::resultJavaScript() const
{
    return "document.getElementsByTagName('textarea')[1].value;\n";
}
