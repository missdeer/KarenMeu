#ifndef TRANSLATEHELPERPAGE_H
#define TRANSLATEHELPERPAGE_H

#include <QWebEnginePage>

enum TranslateService
{
    TST_GOOGLE,
    TST_BAIDU,
    TST_YOUDAO,
    TST_SOGOU,
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
signals:
    void translated(QString);
    void failed();
private slots:

    void onLoadFinished(bool ok);

private:
    TranslateService     m_service;
    TranslateHelperState m_state {THS_IDLE};
    QString              m_originalText;

    void requestYoudao();
    void requestGoogle();
    void requestBaidu();
    void requestSogou();

    void doneYoudao();
    void doneGoogle();
    void doneBaidu();
    void doneSogou();
};

#endif // TRANSLATEHELPERPAGE_H
