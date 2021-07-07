#ifndef TRANSLATEOUTPUTWIDGET_H
#define TRANSLATEOUTPUTWIDGET_H

#include <QWidget>
#include <functional>

#include "translatehelperpage.h"

QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);
QT_FORWARD_DECLARE_CLASS(QLineEdit);
QT_FORWARD_DECLARE_CLASS(QToolBar);
QT_FORWARD_DECLARE_CLASS(QComboBox);

class ITranslator;
class TranslateOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TranslateOutputWidget(ITranslator *translator, QWidget *parent = nullptr);
    ~TranslateOutputWidget();

    QPlainTextEdit *editor() const;
    QLineEdit *     originalTextEditor() const;
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
    QPlainTextEdit *         m_editor {nullptr};
    QLineEdit *              m_originalTextEditor {nullptr};
    QToolBar *               m_toolbar {nullptr};
    QComboBox *              m_fromLanguages {nullptr};
    QComboBox *              m_toLanguages {nullptr};
    TranslateHelperPage *    m_helper {nullptr};
    ITranslator *            m_translator {nullptr};
    std::function<QString()> m_getSelection;
    void                     initializeToolbar();
};

#endif // TRANSLATEOUTPUTWIDGET_H
