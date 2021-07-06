#include <QTimer>
#include <QWebEnginePage>

#include "baidutranslator.h"

QStringList BaiduTranslator::fromLanguages()
{
    return QStringList();
}

QStringList BaiduTranslator::toLanguages()
{
    return QStringList();
}

QString BaiduTranslator::defaultFrom()
{
    return "en";
}

QString BaiduTranslator::defaultTo()
{
    return "zh";
}

void BaiduTranslator::from(const QString &from)
{
    m_from = from;
}

void BaiduTranslator::to(const QString &to)
{
    m_to = to;
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
    return "https://fanyi.baidu.com/#en/zh/";
}

QString BaiduTranslator::resultJavaScript()
{
    return "document.getElementsByClassName('target-output')[0].innerText;\n";
}
