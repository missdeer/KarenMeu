#include <QVBoxLayout>

#include "previewthemeeditor.h"

#include "ScintillaEdit.h"
#include "scintillaconfig.h"

PreviewThemeEditor::PreviewThemeEditor(QWidget *parent) : QWidget(parent), m_editor(new ScintillaEdit(this)), m_sc(new ScintillaConfig(m_editor)) {}

void PreviewThemeEditor::initialize(const QString &lexer)
{
    m_lexer = lexer;
    Q_ASSERT(m_sc);
    m_sc->initScintilla();
    m_sc->initEditorMargins();
    m_sc->initLexerStyle(lexer);

    Q_ASSERT(m_editor);
    auto layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_editor);
    setLayout(layout);
    connect(m_editor, &ScintillaEdit::modified, this, &PreviewThemeEditor::modified);
}

void PreviewThemeEditor::setContent(const QByteArray &content)
{
    Q_ASSERT(m_editor);
    m_editor->setText(content.data());

    m_editor->emptyUndoBuffer();
    Q_ASSERT(m_sc);
    m_sc->initLexerStyle(m_lexer);
    m_editor->colourise(0, -1);
}

QByteArray PreviewThemeEditor::content()
{
    Q_ASSERT(m_editor);
    auto len = m_editor->length() + 1;
    return m_editor->getText(len);
}

void PreviewThemeEditor::clearAll()
{
    Q_ASSERT(m_editor);
    m_editor->clearAll();
}

void PreviewThemeEditor::modified(
    int type, int /*position*/, int /*length*/, int /*linesAdded*/, const QByteArray & /*text*/, int /*line*/, int /*foldNow*/, int /*foldPrev*/)
{
    if (type & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
        emit contentModified();
}
