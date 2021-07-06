#include <functional>

#include <QTimer>

#include "translatehelperpage.h"
#include "settings.h"
#include "translator.hpp"

TranslateHelperPage::TranslateHelperPage(ITranslator *translator, QObject *parent)
    : QWebEnginePage(parent), m_translator(translator), m_timer(new QTimer(this))
{
    connect(this, &QWebEnginePage::loadFinished, this, &TranslateHelperPage::onLoadFinished);
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
    load(QUrl::fromUserInput(m_translator->landingPageUrl() + text.toUtf8()));
    m_timer->start(g_settings->translateTimeout());
#if defined(ENABLE_LOGS)
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << text;
#endif
}

void TranslateHelperPage::getResult()
{
#if defined(ENABLE_LOGS)
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << m_resultJavascript;
#endif
    m_timer->stop();
    runJavaScript(m_translator->resultJavaScript(), [this](const auto &v) {
#if defined(ENABLE_LOGS)
        qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << v;
#endif
        if (v.toString().isEmpty())
        {
            int interval = m_timer->interval() + intervalStartupStep;
            if (interval > g_settings->translateTimeout())
                interval = intervalStartupStep;
            m_resultTryCount++;
            if (m_resultTryCount < 3)
            {
                m_timer->start(interval);
            }
            else
            {
                emit translated(tr("<h4>Translating failed :-(</h4>"));
            }
        }
        else
        {
            m_resultTryCount = 0;
            QString result = QString("<h4>%1</h4><p>%2</p><hr><h4>%3</h4><p>%4</p>")
                                 .arg(tr("Translated Text:"), v.toString(), tr("Original Text:"), m_originalText);
            emit translated(result);
        }
    });
}

void TranslateHelperPage::onLoadFinished(bool ok)
{
#if defined(ENABLE_LOGS)
    qDebug() << __FUNCTION__ << __LINE__ << m_state << m_service << ok;
#endif
    switch (m_state)
    {
    case THS_LOADINGPAGE:
        if (ok)
        {
            m_state = THS_TRANSLATING;
            m_translator->request(this, m_timer, m_originalText.replace("\"", "\\\""));
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
