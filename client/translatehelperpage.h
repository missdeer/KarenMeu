#ifndef TRANSLATEHELPERPAGE_H
#define TRANSLATEHELPERPAGE_H

#include <QWebEnginePage>
QT_FORWARD_DECLARE_CLASS(QTimer);

class ITranslator;

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
    explicit TranslateHelperPage(ITranslator *translator, QObject *parent = nullptr);

    void translate(const QString &text);

public slots:
    void getResult();

private slots:
    void onLoadFinished(bool ok);

signals:
    void translated(QString);
    void failed();

private:
    ITranslator *         m_translator;
    TranslateHelperState  m_state {THS_IDLE};
    QString               m_originalText;
    QTimer *              m_timer;
    int                   m_resultTryCount {0};
};

#endif // TRANSLATEHELPERPAGE_H
