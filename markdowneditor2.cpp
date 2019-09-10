#include <QRegularExpression>
#include <QTextDocumentFragment>
#include "settings.h"
#include "markdowneditor2.h"

MarkdownEditor2::MarkdownEditor2(QWidget *parent)
    :QMarkdownTextEdit(parent)
{
    connect(this, &QPlainTextEdit::modificationChanged, [this](bool changed){ 
        if (changed) 
            emit contentModified();
    });
    connect(this, &QPlainTextEdit::textChanged, this, &MarkdownEditor2::contentModified);
}

void MarkdownEditor2::updateCodeEditorFont()
{
    QFont f(font());
    f.setFamily(g_settings->codeEditorFontFamily());
    f.setPixelSize(g_settings->codeEditorFontPixelSize());
    setFont(f);    
}

void MarkdownEditor2::initialize()
{
    updateCodeEditorFont();
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
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
    return document()->isModified();
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
    applyFormatter("**");
}

void MarkdownEditor2::formatEmphasize()
{
    applyFormatter("*");
}

void MarkdownEditor2::formatStrikethrough()
{
    applyFormatter("~~");
}

void MarkdownEditor2::formatInlineCode()
{
    applyFormatter("`");
}

void MarkdownEditor2::formatCodeBlock()
{
    QTextCursor c = textCursor();

    if (!c.hasSelection())
    {
        // insert multi-line code block if cursor is in an empty line
        if (c.atBlockStart() && c.atBlockEnd()) 
        {
            c.insertText("```\n\n```");
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
        } 
        else 
        {
            c.insertText("``");
        }

        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        setTextCursor(c);
    } 
    else 
    {
        bool addNewline = false;
        
        QString selectedText = c.selection().toPlainText();
        // if the selected text has multiple lines add a multi-line code block
        if (selectedText.contains("\n")) 
        {
            // add an other newline if there is no newline at the end of the
            // selected text
            QString endNewline = selectedText.endsWith("\n") ? "" : "\n";

            selectedText = "``\n" + selectedText + endNewline + "``";
            addNewline = true;
        }

        c.insertText("`" + selectedText + "`");

        if (addNewline) 
        {
            c.insertText("\n");
        }
    }
}

void MarkdownEditor2::formatComment()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
    {
        // insert <!--|-->
        cursor.insertText("<!--\n\n-->\n");
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 5); 
        setTextCursor(cursor);
    }
    else
    {
        QString text = cursor.selectedText();
        // this only applies to the start of the selected block
        text.replace(QRegularExpression("^"), "> ");
        
    }
}

void MarkdownEditor2::formatOrderedList()
{
    formatHeading("* ");
}

void MarkdownEditor2::formatUnorderedList()
{
    QTextCursor c = textCursor();
    if (!c.hasSelection()) 
    {
        c.insertText("1. ");
    } 
    else
    {
        
    }
    setTextCursor(c);
}

void MarkdownEditor2::formatBlockquote()
{
    formatHeading("> ");
}

void MarkdownEditor2::formatHyperlink()
{
    QTextCursor c = textCursor();
        
    if (!c.hasSelection()) 
    {
        c.insertText("[]()");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
    } 
    else 
    {
        QString selectedText = c.selectedText();
        QRegularExpressionMatch match =
                QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch()) 
        {
            c.insertText(match.captured(1) + "[" + match.captured(2) + "]()" + match.captured(3));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    setTextCursor(c);
}

void MarkdownEditor2::formatImage()
{
    QTextCursor c = textCursor();
        
    if (!c.hasSelection()) 
    {
        c.insertText("![]()");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
    } 
    else 
    {
        QString selectedText = c.selectedText();
        QRegularExpressionMatch match =
                QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch()) 
        {
            c.insertText(match.captured(1) + "![" + match.captured(2) + "]()" + match.captured(3));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    setTextCursor(c);
}

void MarkdownEditor2::formatNewParagraph()
{
    
}

void MarkdownEditor2::formatHorizontalRule()
{
    
}

void MarkdownEditor2::formatHeader1()
{
    formatHeading("# ");
}

void MarkdownEditor2::formatHeader2()
{
    formatHeading("## ");
}

void MarkdownEditor2::formatHeader3()
{
    formatHeading("### ");
}

void MarkdownEditor2::formatHeader4()
{
    formatHeading("#### ");
}

void MarkdownEditor2::formatHeader5()
{
    formatHeading("##### ");
}

void MarkdownEditor2::formatHeader6()
{
    formatHeading("###### ");
}

void MarkdownEditor2::formatShiftRight()
{
    formatHeading("  ");
}

void MarkdownEditor2::formatShiftLeft()
{
    
}

bool MarkdownEditor2::undoFormatting(const QString &formatter) 
{
    QTextCursor c = textCursor();
    QString selectedText = c.selectedText();
    int formatterLength = formatter.length();
    int selectionStart = c.selectionStart();
    int selectionEnd = c.selectionEnd();
    
    c.setPosition(selectionStart - formatterLength);
    c.setPosition(selectionEnd + formatterLength, QTextCursor::KeepAnchor);
    QString selectedTextWithFormatter = c.selectedText();
    
    // if the formatter characters were found we remove them
    if (selectedTextWithFormatter.startsWith(formatter) &&
            selectedTextWithFormatter.endsWith(formatter)) 
    {
        c.insertText(selectedText);
        return true;
    }
    
    return false;
}

void MarkdownEditor2::applyFormatter(const QString &formatter) 
{
    QTextCursor c = textCursor();
        
    if (!c.hasSelection()) 
    {
        c.insertText(formatter.repeated(2));
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor,
                       formatter.length());
    } 
    else 
    {
        // first try to undo an existing formatting
        if (undoFormatting(formatter)) 
        {
            return;
        }
        QString selectedText = c.selectedText();
        QRegularExpressionMatch match =
                QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch()) 
        {
            c.insertText(match.captured(1) + formatter + match.captured(2) +
                         formatter + match.captured(3));
        }
    }
    setTextCursor(c);
}

int MarkdownEditor2::currentLineNumber()
{
    QTextCursor cursor = textCursor();
    
    QTextDocument *doc = document();
    QTextBlock blk = doc->findBlock(cursor.position());
    QTextBlock blk2 = doc->begin();
    
    int i = 1;
    while ( blk != blk2 ) {
        blk2 = blk2.next();
        i++;
    }
    
    return i;
}

void MarkdownEditor2::replaceCurrentLineText(const QString &text)
{    
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.insertText(text);    
}

void MarkdownEditor2::formatHeading(const QString &heading)
{
    QTextCursor c = textCursor();
    if (!c.hasSelection()) 
    {
        c.movePosition(QTextCursor::StartOfLine);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        c.insertText(heading + c.selectedText());
        setTextCursor(c);
    } 
    else 
    {
        QString selectedText = c.selectedText();
        // this only applies to the start of the selected block
        selectedText.replace(QRegularExpression("^"), heading);

        // transform Unicode line endings
        // this newline character seems to be used in multi-line selections
        QString newLine = QString::fromUtf8(QByteArray::fromHex("e280a9"));
        selectedText.replace(newLine, "\n" + heading);

        // remove the block quote if it was placed at the end of the text
        selectedText.remove(QRegularExpression(heading + "$"));

        c.insertText(selectedText);
    }
}
