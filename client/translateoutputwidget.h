#ifndef TRANSLATEOUTPUTWIDGET_H
#define TRANSLATEOUTPUTWIDGET_H

#include <QWidget>
#include <functional>

#include "translatehelperpage.h"

QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);
QT_FORWARD_DECLARE_CLASS(QToolBar);

class Provide;
class TranslateOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TranslateOutputWidget(Provider *provider, QWidget *parent = nullptr);
    ~TranslateOutputWidget();

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
    Provider *               m_provider {nullptr};
    std::function<QString()> m_getSelection;
    void                     initializeToolbar();
};

#endif // TRANSLATEOUTPUTWIDGET_H
