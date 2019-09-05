#include "markdowneditor.h"

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : ScintillaEdit(parent)
    , m_sc(this)
{
}

void MarkdownEditor::initialize()
{
    m_sc.initScintilla();
    m_sc.initEditorMargins();
    m_sc.initLexerStyle("markdown");
    m_sc.initEditorFolderStyle();
    
    connect(this, &ScintillaEdit::linesAdded, this, &MarkdownEditor::linesAdded);
    connect(this, &ScintillaEdit::marginClicked, this, &MarkdownEditor::marginClicked);
    connect(this, &ScintillaEdit::modified, this, &MarkdownEditor::modified);
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

QByteArray MarkdownEditor::content()
{
    return getText(textLength() + 1);
}

void MarkdownEditor::formatStrong()
{
    
}

void MarkdownEditor::formatEmphasize()
{
    
}

void MarkdownEditor::formatInlineCode()
{
    
}

void MarkdownEditor::formatComment()
{
    
}

void MarkdownEditor::formatOrderedList()
{
    
}

void MarkdownEditor::formatUnorderedList()
{
    
}

void MarkdownEditor::formatBlockquote()
{
    
}

void MarkdownEditor::formatHyperlink()
{
    
}

void MarkdownEditor::formatImage()
{
    
}

void MarkdownEditor::formatNewParagraph()
{
    
}

void MarkdownEditor::formatHorizontalRule()
{
    
}

void MarkdownEditor::formatHeader1()
{
    
}

void MarkdownEditor::formatHeader2()
{
    
}

void MarkdownEditor::formatHeader3()
{
    
}

void MarkdownEditor::formatHeader4()
{
    
}

void MarkdownEditor::formatHeader5()
{
    
}

void MarkdownEditor::formatHeader6()
{
    
}

void MarkdownEditor::linesAdded(int /*linesAdded*/)
{
    auto* sci = qobject_cast<ScintillaEdit*>(sender());
    sptr_t line_count = sci->lineCount();
    sptr_t left = sci->marginLeft() + 2;
    sptr_t right = sci->marginRight() + 2;
    sptr_t width = left + right + sci->textWidth(STYLE_LINENUMBER, QString("%1").arg(line_count).toStdString().c_str());
    if (width > sci->marginWidthN(0))
        sci->setMarginWidthN(0, width);
}

void MarkdownEditor::marginClicked(int position, int /*modifiers*/, int margin)
{
    auto* sci = qobject_cast<ScintillaEdit*>(sender());
    if (sci->marginTypeN(margin) == SC_MARGIN_SYMBOL)
    {
        sptr_t maskN = sci->marginMaskN(margin);
        if ((maskN & 0xFFFFFFFF) == SC_MASK_FOLDERS)
        {
            sptr_t line = sci->lineFromPosition(position);
            sptr_t foldLevel = sci->foldLevel(line);
            if (foldLevel & SC_FOLDLEVELHEADERFLAG)
            {
                sci->toggleFold(line);
            }
        }
    }
}

void MarkdownEditor::modified(int type, int /*position*/, int /*length*/, int /*linesAdded*/, const QByteArray &/*text*/, int /*line*/, int /*foldNow*/, int /*foldPrev*/)
{
    if (type & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
        emit contentModified();
}
