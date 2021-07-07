#include <QTimer>
#include <QWebEnginePage>

#include "baidutranslator.h"

QStringList BaiduTranslator::fromLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/baidu.json", "fromLanguages");
}

QStringList BaiduTranslator::toLanguages()
{
    return TranslatorUtil::StringList(":/rc/translators/baidu.json", "toLanguages");
}

QString BaiduTranslator::defaultFrom()
{
    return TranslatorUtil::String(":/rc/translators/baidu.json", "defaultFrom");
}

QString BaiduTranslator::defaultTo()
{
    return TranslatorUtil::String(":/rc/translators/baidu.json", "defaultTo");
}

void BaiduTranslator::from(const QString &from)
{
    m_from = from;
}

void BaiduTranslator::to(const QString &to)
{
    m_to = to;
}

QString BaiduTranslator::from()
{
    return TranslatorUtil::String(":/rc/translators/baidu.json", m_from);
}

QString BaiduTranslator::to()
{
    return TranslatorUtil::String(":/rc/translators/baidu.json", m_to);
}

void BaiduTranslator::request(QWebEnginePage *page, QTimer *timer, const QString &originalText)
{
    page->runJavaScript(QString("document.getElementById(\"baidu_translate_input\").value= \"%1\";\n"
                                "document.getElementById(\"translate-button\").click();\n")
                            .arg(originalText),
                        [timer](const QVariant &) { timer->start(intervalStartupStep); });
}

QString BaiduTranslator::landingPageUrl()
{
    return QString("https://fanyi.baidu.com/#%1/%2/").arg(from(), to());
}

QString BaiduTranslator::resultJavaScript()
{
    return "document.getElementsByClassName('target-output')[0].innerText;\n";
}
