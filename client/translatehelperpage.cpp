#include <QTimer>
#include <functional>

#include "translatehelperpage.h"

#include "settings.h"

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

    std::map<TranslateService, std::function<void()>> resultMap = {
        {TST_GOOGLE, std::bind(&TranslateHelperPage::resultGoogle, this)},
        {TST_BAIDU, std::bind(&TranslateHelperPage::resultBaidu, this)},
        {TST_SOGOU, std::bind(&TranslateHelperPage::resultSogou, this)},
        {TST_YOUDAO, std::bind(&TranslateHelperPage::resultYoudao, this)},
        {TST_DEEPL, std::bind(&TranslateHelperPage::resultDeepL, this)},
    };
    m_result = resultMap[ts];

    connect(m_timer, &QTimer::timeout, this, &TranslateHelperPage::getResult);
    m_timer->setSingleShot(true);
    m_timer->setInterval(g_settings->translateTimeout());
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
    m_timer->start();
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << text;
}

void TranslateHelperPage::getResult()
{
    m_timer->stop();
    m_result();
}

void TranslateHelperPage::onLoadFinished(bool ok)
{
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << ok;
    switch (m_state)
    {
    case THS_LOADINGPAGE:
        m_state = THS_TRANSLATING;
        if (ok)
        {
            m_request();
        }
        else
        {
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
                  [this](const QVariant &) {
                      QTimer::singleShot(1000, [this]() {
                          runJavaScript("document.getElementById(\"transTarget\").innerText;\n", [this](const QVariant &v2) {
                              m_timer->stop();
                              emit translated(v2.toString());
                          });
                      });
                  });
}

void TranslateHelperPage::requestGoogle()
{
    QTimer::singleShot(1000, [this]() {
        runJavaScript("document.getElementsByClassName(\"VIiyi\")[0].innerText;\n", [this](const QVariant &v2) {
            m_timer->stop();
            emit translated(v2.toString());
        });
    });
}

void TranslateHelperPage::requestBaidu()
{
    runJavaScript(QString("document.getElementById(\"baidu_translate_input\").value= \"%1\";\n"
                          "document.getElementById(\"translate-button\").click();\n")
                      .arg(m_originalText.replace("\"", "\\\"")),
                  [this](const QVariant &) {
                      QTimer::singleShot(1000, [this]() {
                          runJavaScript("document.getElementsByClassName(\"target-output\")[0].innerText;\n", [this](const QVariant &v2) {
                              m_timer->stop();
                              emit translated(v2.toString());
                          });
                      });
                  });
}

void TranslateHelperPage::requestSogou()
{
    runJavaScript(QString("document.getElementById(\"trans-input\").value= \"%1\";\n"
                          "document.getElementById(\"trans-input\").dispatchEvent("
                          "new KeyboardEvent('keydown', {bubbles: true, cancelable: true, keyCode: 13}));\n")
                      .arg(m_originalText.replace("\"", "\\\"")),
                  [this](const QVariant &) {
                      QTimer::singleShot(1000, [this]() {
                          runJavaScript("document.getElementsByClassName(\"output\")[0].innerText;\n", [this](const QVariant &v2) {
                              m_timer->stop();
                              emit translated(v2.toString());
                          });
                      });
                  });
}

void TranslateHelperPage::requestDeepL()
{
    QTimer::singleShot(1000, [this]() {
        runJavaScript("document.getElementsByTagName(\"textarea\")[1].value;\n", [this](const QVariant &v2) {
            m_timer->stop();
            emit translated(v2.toString());
        });
    });
}

void TranslateHelperPage::resultYoudao()
{
    runJavaScript("document.getElementById(\"transTarget\").innerText;\n", [this](const QVariant &v2) { emit translated(v2.toString()); });
}

void TranslateHelperPage::resultGoogle()
{
    runJavaScript("document.getElementsByClassName(\"VIiyi\")[0].innerText;\n", [this](const QVariant &v2) { emit translated(v2.toString()); });
}

void TranslateHelperPage::resultBaidu()
{
    runJavaScript("document.getElementsByClassName(\"target-output\")[0].innerText;\n",
                  [this](const QVariant &v2) { emit translated(v2.toString()); });
}

void TranslateHelperPage::resultSogou()
{
    runJavaScript("document.getElementsByClassName(\"output\")[0].innerText;\n", [this](const QVariant &v2) { emit translated(v2.toString()); });
}

void TranslateHelperPage::resultDeepL()
{
    runJavaScript("document.getElementsByTagName(\"textarea\")[1].value;\n", [this](const QVariant &v2) { emit translated(v2.toString()); });
}
