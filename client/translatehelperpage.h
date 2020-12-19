#ifndef TRANSLATEHELPERPAGE_H
#define TRANSLATEHELPERPAGE_H

#include <QWebEnginePage>
QT_FORWARD_DECLARE_CLASS(QTimer);

enum TranslateService
{
    TST_GOOGLE,
    TST_BAIDU,
    TST_YOUDAO,
    TST_SOGOU,
    TST_DEEPL,
};

enum TranslateHelperState
{
    THS_LOADINGPAGE,
    THS_TRANSLATING,
    THS_DONE,
    THS_IDLE,
};

class TranslateHelperPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit TranslateHelperPage(TranslateService ts, QObject *parent = nullptr);

    void translate(const QString &text);

public slots:
    void getResult();

private slots:

    void onLoadFinished(bool ok);
signals:
    void translated(QString);

private:
    TranslateService      m_service;
    TranslateHelperState  m_state {THS_IDLE};
    QString               m_originalText;
    QString               m_landingPage;
    std::function<void()> m_request;
    std::function<void()> m_result;
    QTimer *              m_timer;

    void requestYoudao();
    void requestGoogle();
    void requestBaidu();
    void requestSogou();
    void requestDeepL();

    void resultYoudao();
    void resultGoogle();
    void resultBaidu();
    void resultSogou();
    void resultDeepL();
};

#endif // TRANSLATEHELPERPAGE_H
