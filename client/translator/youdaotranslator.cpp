#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QWebEnginePage>

#include "youdaotranslator.h"

QStringList YoudaoTranslator::fromLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/youdao.json", "fromLanguages");
}

QStringList YoudaoTranslator::toLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/youdao.json", "toLanguages");
}

QString YoudaoTranslator::defaultFrom() const
{
    return TranslatorUtil::String(":/rc/translators/youdao.json", "defaultFrom");
}

QString YoudaoTranslator::defaultTo() const
{
    return TranslatorUtil::String(":/rc/translators/youdao.json", "defaultTo");
}

void YoudaoTranslator::from(const QString &from)
{
    m_from = from;
}

void YoudaoTranslator::to(const QString &to)
{
    m_to = to;
}

QString YoudaoTranslator::from() const
{
    return TranslatorUtil::String(":/rc/translators/youdao.json", m_from);
}

QString YoudaoTranslator::to() const
{
    return TranslatorUtil::String(":/rc/translators/youdao.json", m_to);
}

void YoudaoTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    QString js = QString("var list = document.getElementsByTagName('li');"
                         "for (var i=0; i < list.length; i++) {"
                         "  if (list[i].getAttribute('data-value') == '%1') {"
                         "    if (list[i].getAttribute('class') != 'selected') {"
                         "      list[i].children[0].click();"
                         "    }"
                         "  }"
                         "}"
                         "document.getElementById('inputOriginal').value= '%2';"
                         "document.getElementById('transMachine').click();\n")
                     .arg(from() + "2" + to(), originalText);
    page->runJavaScript(js, [timer](const QVariant &v) { timer->start(intervalStartupStep); });
}

QString YoudaoTranslator::landingPageUrl() const
{
    return "https://fanyi.youdao.com/?keyword=";
}

QString YoudaoTranslator::resultJavaScript() const
{
    return "document.getElementById('transTarget').innerText;\n";
}

bool YoudaoTranslator::mustChinese() const
{
    return true;
}
