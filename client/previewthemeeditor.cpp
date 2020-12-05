#include "previewthemeeditor.h"
#include "scintillaconfig.h"

PreviewThemeEditor::PreviewThemeEditor(QWidget *parent) : ScintillaEdit(parent), m_sc(new ScintillaConfig(this)) {}

void PreviewThemeEditor::initialize()
{
    m_sc->initScintilla();
    m_sc->initEditorMargins();
    m_sc->initLexerStyle("css");

    connect(this, &ScintillaEdit::modified, this, &PreviewThemeEditor::modified);
}

void PreviewThemeEditor::setContent(const QByteArray &content)
{
    setText(content.data());

    emptyUndoBuffer();
    m_sc->initLexerStyle("css");
    colourise(0, -1);
}

QByteArray PreviewThemeEditor::content()
{
    auto len = length() + 1;
    return getText(len);
}

void PreviewThemeEditor::modified(
    int type, int /*position*/, int /*length*/, int /*linesAdded*/, const QByteArray & /*text*/, int /*line*/, int /*foldNow*/, int /*foldPrev*/)
{
    if (type & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
        emit contentModified();
}
