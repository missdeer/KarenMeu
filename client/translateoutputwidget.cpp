#include <QPlainTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

#include "translateoutputwidget.h"

#include "clientutils.h"

TranslateOutputWidget::TranslateOutputWidget(Provider *provider, QWidget *parent)
    : QWidget(parent), m_editor(new QPlainTextEdit(this)), m_toolbar(new QToolBar(this)), m_provider(provider)
{
    auto toolLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(0);

    toolLayout->addWidget(m_toolbar);
    toolLayout->addWidget(m_editor);

    setLayout(toolLayout);

    initializeToolbar();
}

TranslateOutputWidget::~TranslateOutputWidget()
{
    delete m_provider;
    m_provider = nullptr;
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
    ClientUtils::setHtmlContent(m_editor, tr("<h4>Translating...</h4>"));
    if (!m_helper)
    {
        m_helper = new TranslateHelperPage(m_provider, this);
        connect(m_helper, &TranslateHelperPage::translated, this, &TranslateOutputWidget::onTranslated);
    }
    Q_ASSERT(m_helper);
    m_helper->translate(text);
}

void TranslateOutputWidget::setGetSelectionCallback(std::function<QString()> f)
{
    m_getSelection = f;
}

QString TranslateOutputWidget::content()
{
    Q_ASSERT(m_editor);
    if (!m_editor->toPlainText().isEmpty())
        return m_editor->document()->toHtml();
    return "";
}

void TranslateOutputWidget::onTranslate()
{
    // get selected text from markdownview
    auto text = m_getSelection();
    translate(text);
}

void TranslateOutputWidget::onRefresh()
{
    if (!m_helper)
    {
        onTranslate();
        return;
    }
    Q_ASSERT(m_helper);
    m_helper->getResult();
}

void TranslateOutputWidget::onTranslated(QString res)
{
    Q_ASSERT(m_editor);
    ClientUtils::setHtmlContent(m_editor, res);
}

void TranslateOutputWidget::initializeToolbar()
{
    m_toolbar->setIconSize(QSize(16, 16));
    auto translateAction = m_toolbar->addAction(QIcon(":/rc/images/translate.png"), tr("Translate"));
    connect(translateAction, &QAction::triggered, this, &TranslateOutputWidget::onTranslate);
    auto refreshAction = m_toolbar->addAction(QIcon(":/rc/images/refresh.png"), tr("Refresh"));
    connect(refreshAction, &QAction::triggered, this, &TranslateOutputWidget::onRefresh);
}
