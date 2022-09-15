#include <QVBoxLayout>

#include "previewthemeeditor.h"
#include "scintillaconfig.h"

PreviewThemeEditor::PreviewThemeEditor(QWidget *parent) : ScintillaEdit(parent), m_sc(new ScintillaConfig(this)) {}

void PreviewThemeEditor::initialize(const QString &lexer)
{
    m_lexer = lexer;
    Q_ASSERT(m_sc);
    m_sc->initScintilla();
    m_sc->initEditorMargins();
    m_sc->initLexerStyle(lexer);

    connect(this, &ScintillaEdit::modified, this, &PreviewThemeEditor::modified);
}

void PreviewThemeEditor::setContent(const QByteArray &content)
{
    setText(content.data());

    emptyUndoBuffer();
    Q_ASSERT(m_sc);
    m_sc->initLexerStyle(m_lexer);
    colourise(0, -1);
}

QByteArray PreviewThemeEditor::content()
{
    auto len = length() + 1;
    return getText(len);
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
    {
        emit contentModified();
    }
}
