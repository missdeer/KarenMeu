#include <QTimer>
#include <QWebEnginePage>

#include "sogoutranslator.h"

QStringList SogouTranslator::fromLanguages()
{
    return QStringList();
}

QStringList SogouTranslator::toLanguages()
{
    return QStringList();
}

QString SogouTranslator::defaultFrom()
{
    return "en";
}

QString SogouTranslator::defaultTo()
{
    return "zh";
}

void SogouTranslator::from(const QString &from)
{
    m_from = from;
}

void SogouTranslator::to(const QString &to)
{
    m_to = to;
}

void SogouTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    page->runJavaScript(QString("document.getElementById(\"trans-input\").value= \"%1\";\n"
                                "document.getElementById(\"trans-input\").dispatchEvent("
                                "new KeyboardEvent('keydown', {bubbles: true, cancelable: true, keyCode: 13}));\n")
                            .arg(originalText),
                        [timer](const QVariant &) { timer->start(intervalStartupStep); });
}

QString SogouTranslator::landingPageUrl()
{
    return "https://fanyi.sogou.com/?transfrom=en&transto=zh-CHS&isclient=1&model=general&keyword=";
}

QString SogouTranslator::resultJavaScript()
{
    return "document.getElementById('trans-result').innerText;\n";
}
