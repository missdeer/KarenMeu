#include <QTimer>
#include <QWebEnginePage>

#include "youdaotranslator.h"

QStringList YoudaoTranslator::fromLanguages()
{
    return QStringList();
}

QStringList YoudaoTranslator::toLanguages()
{
    return QStringList();
}

QString YoudaoTranslator::defaultFrom()
{
    return "en";
}

QString YoudaoTranslator::defaultTo()
{
    return "zh";
}

void YoudaoTranslator::from(const QString &from)
{
    m_from = from;
}

void YoudaoTranslator::to(const QString &to)
{
    m_to = to;
}

void YoudaoTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    page->runJavaScript(QString("document.getElementById('inputOriginal').value= '%1';"
                                "document.getElementById('transMachine').click();\n")
                            .arg(originalText),
                        [timer](const QVariant &v) { timer->start(intervalStartupStep); });
}

QString YoudaoTranslator::landingPageUrl()
{
    return "https://fanyi.youdao.com/?keyword=";
}

QString YoudaoTranslator::resultJavaScript()
{
    return "document.getElementById('transTarget').innerText;\n";
}
