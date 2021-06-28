#include <QVBoxLayout>

#include "previewthemeeditor.h"
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

void PreviewThemeEditor::modified(Scintilla::ModificationFlags type,
                                  Scintilla::Position          position,
                                  Scintilla::Position          length,
                                  Scintilla::Position          linesAdded,
                                  const QByteArray &           text,
                                  Scintilla::Position          line,
                                  Scintilla::FoldLevel         foldNow,
                                  Scintilla::FoldLevel         foldPrev)
{
    if ((int)type & (int)(Scintilla::ModificationFlags::InsertText | Scintilla::ModificationFlags::DeleteText | Scintilla::ModificationFlags::Undo |
                          Scintilla::ModificationFlags::Redo | Scintilla::ModificationFlags::MultilineUndoRedo))
        emit contentModified();
}
