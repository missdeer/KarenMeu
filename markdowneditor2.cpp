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
    QString selectedText = c.selection().toPlainText();

    if (selectedText.isEmpty())
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
        
    }
}

void MarkdownEditor2::formatOrderedList()
{
    
}

void MarkdownEditor2::formatUnorderedList()
{
    
}

void MarkdownEditor2::formatBlockquote()
{
    QTextCursor c = textCursor();
    if (!c.hasSelection()) 
    {
        c.insertText("> ");
        setTextCursor(c);
    } 
    else 
    {
        QString selectedText = c.selectedText();
        // this only applies to the start of the selected block
        selectedText.replace(QRegularExpression("^"), "> ");

        // transform Unicode line endings
        // this newline character seems to be used in multi-line selections
        QString newLine = QString::fromUtf8(QByteArray::fromHex("e280a9"));
        selectedText.replace(newLine, "\n> ");

        // remove the block quote if it was placed at the end of the text
        selectedText.remove(QRegularExpression("> $"));

        c.insertText(selectedText);
    }
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
        setTextCursor(c);
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
}
