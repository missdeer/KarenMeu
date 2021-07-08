#include <QTimer>
#include <QWebEnginePage>

#include "sogoutranslator.h"

QStringList SogouTranslator::fromLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/sogou.json", "fromLanguages");
}

QStringList SogouTranslator::toLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/sogou.json", "toLanguages");
}

QString SogouTranslator::defaultFrom() const
{
    return TranslatorUtil::String(":/rc/translators/sogou.json", "defaultFrom");
}

QString SogouTranslator::defaultTo() const
{
    return TranslatorUtil::String(":/rc/translators/sogou.json", "defaultTo");
}

void SogouTranslator::from(const QString &from)
{
    m_from = from;
}

void SogouTranslator::to(const QString &to)
{
    m_to = to;
}

QString SogouTranslator::from() const
{
    return TranslatorUtil::String(":/rc/translators/sogou.json", m_from);
}

QString SogouTranslator::to() const
{
    return TranslatorUtil::String(":/rc/translators/sogou.json", m_to);
}

void SogouTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    page->runJavaScript(QString("document.getElementById(\"trans-input\").value= \"%1\";\n"
                                "document.getElementById(\"trans-input\").dispatchEvent("
                                "new KeyboardEvent('keydown', {bubbles: true, cancelable: true, keyCode: 13}));\n")
                            .arg(originalText),
                        [timer](const QVariant &) { timer->start(intervalStartupStep); });
}

QString SogouTranslator::landingPageUrl() const
{
    return QString("https://fanyi.sogou.com/?transfrom=%1&transto=%2&isclient=1&model=general&keyword=").arg(from(), to());
}

QString SogouTranslator::resultJavaScript() const
{
    return "document.getElementById('trans-result').innerText;\n";
}
