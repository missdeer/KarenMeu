#include "scintillaconfig.h"
#include "previewthemeeditor.h"

PreviewThemeEditor::PreviewThemeEditor(QWidget *parent)
    : ScintillaEdit(parent)
    , m_sc(new ScintillaConfig(this))
{
}

void PreviewThemeEditor::initialize()
{
    m_sc->initScintilla();
    m_sc->initLexerStyle("css");
}

void PreviewThemeEditor::setContent(const QByteArray &content)
{
    setText(content.data());

    emptyUndoBuffer();
    m_sc->initLexerStyle("css");
    colourise(0, -1);
}
