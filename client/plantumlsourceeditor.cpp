#include <QVBoxLayout>

#include "plantumlsourceeditor.h"
#include "scintillaconfig.h"

PlantUMLSourceEditor::PlantUMLSourceEditor(QWidget *parent) : ScintillaEdit {parent}, m_sc(new ScintillaConfig(this)) {}

void PlantUMLSourceEditor::initialize()
{
    Q_ASSERT(m_sc);
    m_sc->initScintilla();
    m_sc->initEditorMargins();
    m_sc->initLexerStyle("cpp");

    connect(this, &ScintillaEdit::modified, this, &PlantUMLSourceEditor::modified);
}

void PlantUMLSourceEditor::setContent(const QByteArray &content)
{
    setText(content.data());

    emptyUndoBuffer();
    Q_ASSERT(m_sc);
    m_sc->initLexerStyle("cpp");
    colourise(0, -1);
}

QByteArray PlantUMLSourceEditor::content()
{
    auto len = length() + 1;
    return getText(len);
}

void PlantUMLSourceEditor::modified(Scintilla::ModificationFlags type,
                                    Scintilla::Position          position,
                                    Scintilla::Position          length,
                                    Scintilla::Position          linesAdded,
                                    const QByteArray            &text,
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
