#include <QTimer>
#include <functional>

#include "translatehelperpage.h"

TranslateHelperPage::TranslateHelperPage(TranslateService ts, QObject *parent) : QWebEnginePage(parent), m_service(ts)
{
    connect(this, &QWebEnginePage::loadFinished, this, &TranslateHelperPage::onLoadFinished);
}

void TranslateHelperPage::translate(const QString &text)
{
    m_originalText                          = text;
    std::map<TranslateService, QUrl> tsUrls = {
        {TST_GOOGLE, QUrl::fromUserInput("https://translate.google.com/?sl=auto&tl=zh-CN&text=" + text.toUtf8().toPercentEncoding())},
        {TST_BAIDU, QUrl::fromUserInput("https://fanyi.baidu.com/#en/zh/" + text.toUtf8().toPercentEncoding())},
        {TST_SOGOU,
         QUrl::fromUserInput("https://fanyi.sogou.com/?transfrom=en&transto=zh-CHS&isclient=1&model=general&keyword=" +
                             text.toUtf8().toPercentEncoding())},
        {TST_YOUDAO, QUrl("http://fanyi.youdao.com/")}};
    m_state = THS_LOADINGPAGE;
    load(tsUrls[m_service]);
    qDebug() << QUrl::fromUserInput("https://fanyi.baidu.com/#en/zh/" + text.toUtf8().toPercentEncoding());
}

void TranslateHelperPage::getResult()
{
    std::map<TranslateService, std::function<void()>> requestMap = {
        {TST_GOOGLE, std::bind(&TranslateHelperPage::requestGoogle, this)},
        {TST_BAIDU, std::bind(&TranslateHelperPage::requestBaidu, this)},
        {TST_SOGOU, std::bind(&TranslateHelperPage::requestSogou, this)},
        {TST_YOUDAO, std::bind(&TranslateHelperPage::requestYoudao, this)},
    };
    auto it = requestMap.find(m_service);
    if (requestMap.end() != it)
    {
        it->second();
    }
}

void TranslateHelperPage::onLoadFinished(bool ok)
{
    qDebug() << __FUNCTION__ << __LINE__ << m_state << ok;
    switch (m_state)
    {
    case THS_LOADINGPAGE:
        m_state = THS_TRANSLATING;
        if (ok)
        {
            qDebug() << __FUNCTION__ << __LINE__ << m_service;
            getResult();
        }
        break;
    case THS_TRANSLATING:
        m_state = THS_DONE;
        if (ok)
        {
            qDebug() << __FUNCTION__ << __LINE__ << m_service;
        }
        m_state = THS_IDLE;
        break;
    default:
        break;
    }
}

void TranslateHelperPage::requestYoudao()
{
    runJavaScript(QString("document.getElementById(\"inputOriginal\").value= \"%1\";\n"
                          "document.getElementById(\"transMachine\").click();\n")
                      .arg(m_originalText.replace("\"", "\\\"")),
                  [this](const QVariant &) {
                      QTimer::singleShot(1000, [this]() {
                          runJavaScript("document.getElementById(\"transTarget\").innerText;\n",
                                        [this](const QVariant &v2) { emit translated(v2.toString()); });
                      });
                  });
}

void TranslateHelperPage::requestGoogle()
{
    runJavaScript("let ele = document.getElementsByClassName(\"VIiyi\");\n ele[0].innerText;\n",
                  [this](const QVariant &v2) { emit translated(v2.toString()); });
}

void TranslateHelperPage::requestBaidu()
{
    QTimer::singleShot(1000, [this]() {
        runJavaScript("let ele = document.getElementsByClassName(\"target-output\");\n ele[0].innerText;\n",
                      [this](const QVariant &v2) { emit translated(v2.toString()); });
    });
}

void TranslateHelperPage::requestSogou()
{
    runJavaScript("let ele = document.getElementsByClassName(\"output\");\n ele[0].innerText;\n",
                  [this](const QVariant &v2) { emit translated(v2.toString()); });
}
