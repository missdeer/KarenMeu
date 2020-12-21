#include <QTimer>
#include <functional>

#include "translatehelperpage.h"

#include "settings.h"

static const int intervalStartupStep = 1000; // millisecond

TranslateHelperPage::TranslateHelperPage(TranslateService ts, QObject *parent) : QWebEnginePage(parent), m_service(ts), m_timer(new QTimer(this))
{
    connect(this, &QWebEnginePage::loadFinished, this, &TranslateHelperPage::onLoadFinished);
    std::map<TranslateService, QString> landingPageUrlMap = {
        {TST_GOOGLE, "https://translate.google.com/?sl=auto&tl=zh-CN&text="},
        {TST_BAIDU, "https://fanyi.baidu.com/#en/zh/"},
        {TST_SOGOU, "https://fanyi.sogou.com/?transfrom=en&transto=zh-CHS&isclient=1&model=general&keyword="},
        {TST_YOUDAO, "http://fanyi.youdao.com/?keyword="},
        {TST_DEEPL, "https://www.deepl.com/translator#en/zh/"}};
    m_landingPage = landingPageUrlMap[ts];

    std::map<TranslateService, std::function<void()>> requestMap = {
        {TST_GOOGLE, std::bind(&TranslateHelperPage::requestGoogle, this)},
        {TST_BAIDU, std::bind(&TranslateHelperPage::requestBaidu, this)},
        {TST_SOGOU, std::bind(&TranslateHelperPage::requestSogou, this)},
        {TST_YOUDAO, std::bind(&TranslateHelperPage::requestYoudao, this)},
        {TST_DEEPL, std::bind(&TranslateHelperPage::requestDeepL, this)},
    };
    m_request = requestMap[ts];

    std::map<TranslateService, QString> resultJavascriptMap = {{TST_GOOGLE, "document.getElementsByClassName(\"VIiyi\")[0].innerText;\n"},
                                                               {TST_BAIDU, "document.getElementsByClassName(\"target-output\")[0].innerText;\n"},
                                                               {TST_SOGOU, "document.getElementsByClassName(\"output\")[0].innerText;\n"},
                                                               {TST_YOUDAO, "document.getElementById(\"transTarget\").innerText;\n"},
                                                               {TST_DEEPL, "document.getElementsByTagName(\"textarea\")[1].value;\n"}};
    m_resultJavascript                                      = resultJavascriptMap[ts];

    connect(m_timer, &QTimer::timeout, this, &TranslateHelperPage::getResult);
    m_timer->setSingleShot(true);
    m_timer->stop();
}

void TranslateHelperPage::translate(const QString &text)
{
    if (m_state == THS_LOADINGPAGE)
    {
        action(QWebEnginePage::Stop);
    }
    m_originalText = text;
    m_state = THS_LOADINGPAGE;
    load(QUrl(m_landingPage + text.toUtf8().toPercentEncoding()));
    m_timer->start(g_settings->translateTimeout());
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << text;
}

void TranslateHelperPage::getResult()
{
    m_timer->stop();
    runJavaScript(m_resultJavascript, [this](const auto &v) {
        if (v.toString().isEmpty())
        {
            int interval = m_timer->interval() + intervalStartupStep;
            if (interval > g_settings->translateTimeout())
                interval = intervalStartupStep;
            m_timer->start(interval);
        }
        else
        {
            QString result = QString("<h4>%1</h4><p>%2</p><hr><h4>%3</h4><p>%4</p>")
                                 .arg(tr("Translated Text:"), v.toString(), tr("Original Text:"), m_originalText);
            emit translated(result);
        }
    });
}

void TranslateHelperPage::onLoadFinished(bool ok)
{
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << ok;
    switch (m_state)
    {
    case THS_LOADINGPAGE:
        if (ok)
        {
            m_state = THS_TRANSLATING;
            m_request();
        }
        else
        {
            m_state = THS_LOADINGPAGE;
            action(QWebEnginePage::Reload);
        }
        break;
    case THS_TRANSLATING:
        m_state = THS_DONE;
        if (ok)
        {
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
                  [this](const QVariant &) { m_timer->start(intervalStartupStep); });
}

void TranslateHelperPage::requestGoogle()
{
    m_timer->start(intervalStartupStep);
}

void TranslateHelperPage::requestBaidu()
{
    runJavaScript(QString("document.getElementById(\"baidu_translate_input\").value= \"%1\";\n"
                          "document.getElementById(\"translate-button\").click();\n")
                      .arg(m_originalText.replace("\"", "\\\"")),
                  [this](const QVariant &) { m_timer->start(intervalStartupStep); });
}

void TranslateHelperPage::requestSogou()
{
    runJavaScript(QString("document.getElementById(\"trans-input\").value= \"%1\";\n"
                          "document.getElementById(\"trans-input\").dispatchEvent("
                          "new KeyboardEvent('keydown', {bubbles: true, cancelable: true, keyCode: 13}));\n")
                      .arg(m_originalText.replace("\"", "\\\"")),
                  [this](const QVariant &) { m_timer->start(intervalStartupStep); });
}

void TranslateHelperPage::requestDeepL()
{
    m_timer->start(intervalStartupStep);
}
