#ifndef TRANSLATEOUTPUTWIDGET_H
#define TRANSLATEOUTPUTWIDGET_H

#include <QWidget>
#include <functional>

#include "translatehelperpage.h"

QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);
QT_FORWARD_DECLARE_CLASS(QToolBar);

class TranslateOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TranslateOutputWidget(TranslateService ts, QWidget *parent = nullptr);

    QPlainTextEdit *editor() const;
    void            clear();
    void            translate(const QString &text);
    void            setGetSelectionCallback(std::function<QString()> f);
    QString         content();
public slots:
    void onTranslated(QString res);
private slots:
    void onTranslate();
    void onRefresh();
signals:

private:
    QPlainTextEdit *         m_editor;
    QToolBar *               m_toolbar;
    TranslateHelperPage *    m_helper {nullptr};
    TranslateService         m_service;
    std::function<QString()> m_getSelection;
    void                     initializeToolbar();
};

#endif // TRANSLATEOUTPUTWIDGET_H
