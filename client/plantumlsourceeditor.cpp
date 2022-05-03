#include <QVBoxLayout>

#include "plantumlsourceeditor.h"
#include "scintillaconfig.h"

PlantUMLSourceEditor::PlantUMLSourceEditor(QWidget *parent) : QWidget {parent}, m_editor(new ScintillaEdit(this)), m_sc(new ScintillaConfig(m_editor))
{
    
}

void PlantUMLSourceEditor::initialize()
{
    Q_ASSERT(m_sc);
    m_sc->initScintilla();
    m_sc->initEditorMargins();
    m_sc->initLexerStyle("cpp");

    Q_ASSERT(m_editor);
    auto layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_editor);
    setLayout(layout);
    connect(m_editor, &ScintillaEdit::modified, this, &PlantUMLSourceEditor::modified);
}

void PlantUMLSourceEditor::setContent(const QByteArray &content)
{
    Q_ASSERT(m_editor);
    m_editor->setText(content.data());

    m_editor->emptyUndoBuffer();
    Q_ASSERT(m_sc);
    m_sc->initLexerStyle("cpp");
    m_editor->colourise(0, -1);
}

QByteArray PlantUMLSourceEditor::content()
{
    Q_ASSERT(m_editor);
    auto len = m_editor->length() + 1;
    return m_editor->getText(len);
}

void PlantUMLSourceEditor::clearAll()
{
    Q_ASSERT(m_editor);
    m_editor->clearAll();
}

void PlantUMLSourceEditor::setReadOnly(bool readOnly)
{
    Q_ASSERT(m_editor);
    m_editor->setReadOnly(readOnly);
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
        emit contentModified();
}
