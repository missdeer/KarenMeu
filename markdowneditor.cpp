#include "markdowneditor.h"

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : ScintillaEdit(parent)
    , m_sc(this)
{
    
}

void MarkdownEditor::initialize()
{
    m_sc.initScintilla();
    m_sc.inttOuputWindowMargins();
    m_sc.initLexerStyle("markdown");
}

void MarkdownEditor::setContent(const QString &content)
{
    auto b = content.toUtf8();
    setContent(b);
}

void MarkdownEditor::setContent(const QByteArray &content)
{
    setText(content.data());

    emptyUndoBuffer();
}
