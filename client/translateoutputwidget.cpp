#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

#include "translateoutputwidget.h"
#include "clientutils.h"
#include "translator.hpp"

TranslateOutputWidget::TranslateOutputWidget(ITranslator *translator, QWidget *parent)
    : QWidget(parent),
      m_editor(new QPlainTextEdit(this)),
      m_originalTextEditor(new QLineEdit(this)),
      m_toolbar(new QToolBar(this)),
      m_fromLanguages(new QComboBox(m_toolbar)),
      m_toLanguages(new QComboBox(m_toolbar)),
      m_translator(translator)
{
    auto toolLayout = new QVBoxLayout(this);
    toolLayout->setContentsMargins(0, 2, 0, 2);
    toolLayout->setSpacing(0);

    toolLayout->addWidget(m_toolbar);
    toolLayout->addWidget(m_editor);

    setLayout(toolLayout);

    initializeToolbar();
    m_fromLanguages->addItems(translator->fromLanguages());
    m_toLanguages->addItems(translator->toLanguages());
    connect(m_fromLanguages, &QComboBox::currentTextChanged, [translator](const QString &text) { translator->from(text); });
    connect(m_toLanguages, &QComboBox::currentTextChanged, [translator](const QString &text) { translator->to(text); });
    m_fromLanguages->setCurrentText(translator->defaultFrom());
    m_toLanguages->setCurrentText(translator->defaultTo());
}

TranslateOutputWidget::~TranslateOutputWidget()
{
    delete m_translator;
    m_translator = nullptr;
}

QPlainTextEdit *TranslateOutputWidget::editor() const
{
    return m_editor;
}

QLineEdit *TranslateOutputWidget::originalTextEditor() const
{
    return m_originalTextEditor;
}

void TranslateOutputWidget::clear()
{
    Q_ASSERT(m_editor);
    m_editor->clear();
}

void TranslateOutputWidget::translate(const QString &text)
{
    ClientUtils::setHtmlContent(m_editor, tr("<h4>Translating...</h4>"));
    m_originalTextEditor->setText(text);
    if (!m_helper)
    {
        m_helper = new TranslateHelperPage(m_translator, this);
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
    m_toolbar->addSeparator();
    m_toolbar->addWidget(new QLabel(tr("From")));
    m_toolbar->addWidget(m_fromLanguages);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(new QLabel(tr("To")));
    m_toolbar->addWidget(m_toLanguages);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(new QLabel(tr("Original Text:"), this));
    m_toolbar->addWidget(m_originalTextEditor);
}
