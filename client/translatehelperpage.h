#ifndef TRANSLATEHELPERPAGE_H
#define TRANSLATEHELPERPAGE_H

#include <QWebEnginePage>
QT_FORWARD_DECLARE_CLASS(QTimer);

enum TranslateHelperState
{
    THS_LOADINGPAGE,
    THS_TRANSLATING,
    THS_DONE,
    THS_IDLE,
};

class Provider
{
public:
    virtual ~Provider()                                                                       = default;
    virtual void    request(QWebEnginePage *page, QTimer *timer, const QString &originalText) = 0;
    virtual QString landingPageUrl()                                                          = 0;
    virtual QString resultJavaScript()                                                        = 0;
};

class YoudaoProvider : public Provider
{
public:
    void    request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString landingPageUrl() override
    {
        return "https://fanyi.youdao.com/?keyword=";
    }
    QString resultJavaScript() override
    {
        return "document.getElementById('transTarget').innerText;\n";
    }
};
class BaiduProvider : public Provider
{
public:
    void    request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString landingPageUrl() override
    {
        return "https://fanyi.baidu.com/#en/zh/";
    }
    QString resultJavaScript() override
    {
        return "document.getElementsByClassName('target-output')[0].innerText;\n";
    }
};
class SogouProvider : public Provider
{
public:
    void request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;

    QString landingPageUrl() override
    {
        return "https://fanyi.sogou.com/?transfrom=en&transto=zh-CHS&isclient=1&model=general&keyword=";
    }

    QString resultJavaScript() override
    {
        return "document.getElementById('trans-result').innerText;\n";
    }
};
class GoogleProvider : public Provider
{
public:
    void    request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString landingPageUrl() override
    {
        return "https://translate.google.com/?sl=auto&tl=zh-CN&text=";
    }
    QString resultJavaScript() override
    {
        return "document.getElementsByClassName('VIiyi')[0].innerText;\n";
    }
};
class DeepLProvider : public Provider
{
public:
    void    request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString landingPageUrl() override
    {
        return "https://www.deepl.com/translator#en/zh/";
    }
    QString resultJavaScript() override
    {
        return "document.getElementsByTagName('textarea')[1].value;\n";
    }
};

class TranslateHelperPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit TranslateHelperPage(Provider *provider, QObject *parent = nullptr);

    void translate(const QString &text);

public slots:
    void getResult();

private slots:
    void onLoadFinished(bool ok);

signals:
    void translated(QString);

private:
    Provider *            m_provider;
    TranslateHelperState  m_state {THS_IDLE};
    QString               m_originalText;
    QTimer *              m_timer;
    int                   m_resultTryCount {0};
};

#endif // TRANSLATEHELPERPAGE_H
