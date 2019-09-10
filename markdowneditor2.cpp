#include "markdowneditor2.h"

MarkdownEditor2::MarkdownEditor2(QWidget *parent)
    :QMarkdownTextEdit(parent)
{
    connect(this, &QPlainTextEdit::modificationChanged, [this](bool changed){ 
        m_modified = changed;
        if (changed) 
            emit contentModified();
    });
    connect(this, &QPlainTextEdit::textChanged, this, &MarkdownEditor2::contentModified);
}

void MarkdownEditor2::initialize()
{
    QFont f(font());
    f.setFamily("Yahei Source Code Pro");
    f.setPixelSize(16);
    setFont(f);
}

void MarkdownEditor2::setContent(const QString &content)
{
    setText(content);
}

void MarkdownEditor2::setContent(const QByteArray &content)
{
    setText(content);
}

QByteArray MarkdownEditor2::content()
{
    return toPlainText().toUtf8();
}

bool MarkdownEditor2::modify()
{
    return m_modified;
}

void MarkdownEditor2::setSavePoint()
{
    document()->setModified(false);
}

void MarkdownEditor2::emptyUndoBuffer()
{
    
}

void MarkdownEditor2::formatStrong()
{
    
}

void MarkdownEditor2::formatEmphasize()
{
    
}

void MarkdownEditor2::formatStrikethrough()
{
    
}

void MarkdownEditor2::formatInlineCode()
{
    
}

void MarkdownEditor2::formatCodeBlock()
{
    
}

void MarkdownEditor2::formatComment()
{
    
}

void MarkdownEditor2::formatOrderedList()
{
    
}

void MarkdownEditor2::formatUnorderedList()
{
    
}

void MarkdownEditor2::formatBlockquote()
{
    
}

void MarkdownEditor2::formatHyperlink()
{
    
}

void MarkdownEditor2::formatImage()
{
    
}

void MarkdownEditor2::formatNewParagraph()
{
    
}

void MarkdownEditor2::formatHorizontalRule()
{
    
}

void MarkdownEditor2::formatHeader1()
{
    
}

void MarkdownEditor2::formatHeader2()
{
    
}

void MarkdownEditor2::formatHeader3()
{
    
}

void MarkdownEditor2::formatHeader4()
{
    
}

void MarkdownEditor2::formatHeader5()
{
    
}

void MarkdownEditor2::formatHeader6()
{
    
}

void MarkdownEditor2::formatShiftRight()
{
    
}

void MarkdownEditor2::formatShiftLeft()
{
    
}
