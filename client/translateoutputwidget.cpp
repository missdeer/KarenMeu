#include <QPlainTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

#include "translateoutputwidget.h"

TranslateOutputWidget::TranslateOutputWidget(TranslateService ts, QWidget *parent)
    : QWidget(parent)
    , m_editor(new QPlainTextEdit(this))
    , m_toolbar(new QToolBar(this))
    , m_service(ts)
{
    auto toolLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(0);

    toolLayout->addWidget(m_toolbar);
    toolLayout->addWidget(m_editor);

    setLayout(toolLayout);

    initializeToolbar();
}

QPlainTextEdit *TranslateOutputWidget::editor() const
{
    return m_editor;
}

void TranslateOutputWidget::clear()
{
    Q_ASSERT(m_editor);
    m_editor->clear();
}

void TranslateOutputWidget::translate(const QString &text)
{
    clear();
    if (!m_helper)
    {
        m_helper = new TranslateHelperPage(m_service, this);
        connect(m_helper, &TranslateHelperPage::translated, this, &TranslateOutputWidget::onTranslated);
    }
    Q_ASSERT(m_helper);
    m_helper->translate(text);
}

void TranslateOutputWidget::setGetSelectionCallback(std::function<QString()> f)
{
    m_getSelection = f;
}

void TranslateOutputWidget::onTranslate()
{
    // get selected text from markdownview
    auto text = m_getSelection();
    translate(text);
}

void TranslateOutputWidget::onRefresh()
{
    m_helper->getResult();
}

void TranslateOutputWidget::onTranslated(QString res)
{
    Q_ASSERT(m_editor);
    m_editor->clear();
    m_editor->appendHtml(res);
    m_editor->moveCursor(QTextCursor::Start);
}

void TranslateOutputWidget::initializeToolbar()
{
    m_toolbar->setIconSize(QSize(16, 16));
    auto translateAction = m_toolbar->addAction(QIcon(":/rc/images/translate.png"), tr("Translate"));
    connect(translateAction, &QAction::triggered, this, &TranslateOutputWidget::onTranslate);
    auto refreshAction = m_toolbar->addAction(QIcon(":/rc/images/refresh.png"), tr("Refresh"));
    connect(refreshAction, &QAction::triggered, this, &TranslateOutputWidget::onRefresh);
}
