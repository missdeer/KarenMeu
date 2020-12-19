#include <QRegularExpression>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextEdit>

#include "markdowneditor4.h"

#include <vtextedit/vtextedit.h>

#include "clientutils.h"
#include "settings.h"

MarkdownEditor4::MarkdownEditor4(const QSharedPointer<vte::MarkdownEditorConfig> &p_config, QWidget *parent) : vte::VMarkdownEditor(p_config, parent)
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    connect(editor, &QTextEdit::textChanged, this, &MarkdownEditor4::contentModified);
    auto scrollBar = editor->verticalScrollBar();
    Q_ASSERT(scrollBar);
    connect(scrollBar, &QScrollBar::valueChanged, [this, scrollBar](int) { emit scrollValueChanged(scrollBar->value(), scrollBar->maximum()); });
    connect(scrollBar, &QScrollBar::rangeChanged, [this, scrollBar](int, int) { emit scrollValueChanged(scrollBar->value(), scrollBar->maximum()); });
}

void MarkdownEditor4::initialize()
{
    vte::VTextEdit *editor = getTextEdit();
    ClientUtils::InitializzWidgetFont(static_cast<QWidget *>(editor));
    setSyntax("breeze-dark.theme");
}

void MarkdownEditor4::setContent(const QString &content)
{
    setText(content);
}

void MarkdownEditor4::setContent(const QByteArray &content)
{
    setText(content);
}

QByteArray MarkdownEditor4::content()
{
    return getText().toUtf8();
}

bool MarkdownEditor4::modify()
{
    return isModified();
}

void MarkdownEditor4::setSavePoint()
{
    document()->setModified(false);
}

void MarkdownEditor4::emptyUndoBuffer() {}

void MarkdownEditor4::clear()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->clear();
}

void MarkdownEditor4::copy()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->copy();
}

void MarkdownEditor4::cut()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->cut();
}

void MarkdownEditor4::paste()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->paste();
}

void MarkdownEditor4::undo()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->undo();
}

void MarkdownEditor4::redo()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->redo();
}

void MarkdownEditor4::selectAll()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->selectAll();
}

void MarkdownEditor4::formatStrong()
{
    applyFormatter("**");
}

void MarkdownEditor4::formatEmphasize()
{
    applyFormatter("*");
}

void MarkdownEditor4::formatStrikethrough()
{
    applyFormatter("~~");
}

void MarkdownEditor4::formatInlineCode()
{
    applyFormatter("`");
}

void MarkdownEditor4::formatCodeBlock()
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
            addNewline   = true;
        }

        c.insertText("`" + selectedText + "`");

        if (addNewline)
        {
            c.insertText("\n");
        }
    }
}

void MarkdownEditor4::formatComment()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
    {
        // insert <!--|-->
        cursor.insertText("<!--\n\n-->\n");
    }
    else
    {
        QString text = cursor.selectedText();
        cursor.insertText("\n<!--\n" + text + "\n-->\n");
    }
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 5);
    setTextCursor(cursor);
}

void MarkdownEditor4::formatOrderedList()
{
    QTextCursor c = textCursor();
    if (!c.hasSelection())
    {
        c.insertText("1. ");
    }
    else
    {
        auto start = c.selectionStart();
        auto end   = c.selectionEnd();
        c.setPosition(end);
        auto endLine = currentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; currentLineNumber(&c) <= endLine; index++)
        {
            c.movePosition(QTextCursor::EndOfLine);
            c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString text = c.selectedText();
            c.insertText(QString("%1. %2").arg(index).arg(text));
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::Down);
        }
    }
    setTextCursor(c);
}

void MarkdownEditor4::formatUnorderedList()
{
    formatHeading("* ");
}

void MarkdownEditor4::formatBlockquote()
{
    formatHeading("> ");
}

void MarkdownEditor4::formatHyperlink()
{
    QTextCursor c = textCursor();

    if (!c.hasSelection())
    {
        c.insertText("[]()");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
    }
    else
    {
        QString                 selectedText = c.selectedText();
        QRegularExpressionMatch match        = QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch())
        {
            c.insertText(match.captured(1) + "[" + match.captured(2) + "]()" + match.captured(3));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    setTextCursor(c);
}

void MarkdownEditor4::formatImage()
{
    QTextCursor c = textCursor();

    if (!c.hasSelection())
    {
        c.insertText("![]()");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
    }
    else
    {
        QString                 selectedText = c.selectedText();
        QRegularExpressionMatch match        = QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch())
        {
            c.insertText(match.captured(1) + "![" + match.captured(2) + "]()" + match.captured(3));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    setTextCursor(c);
}

void MarkdownEditor4::formatNewParagraph()
{
    QTextCursor c = textCursor();
    c.insertText("\n");

    setTextCursor(c);
}

void MarkdownEditor4::formatHorizontalRule()
{
    QTextCursor c = textCursor();
    c.insertText("---\n");

    setTextCursor(c);
}

void MarkdownEditor4::formatHeader1()
{
    formatHeading("# ");
}

void MarkdownEditor4::formatHeader2()
{
    formatHeading("## ");
}

void MarkdownEditor4::formatHeader3()
{
    formatHeading("### ");
}

void MarkdownEditor4::formatHeader4()
{
    formatHeading("#### ");
}

void MarkdownEditor4::formatHeader5()
{
    formatHeading("##### ");
}

void MarkdownEditor4::formatHeader6()
{
    formatHeading("###### ");
}

void MarkdownEditor4::formatShiftRight()
{
    formatHeading("  ");
}

void MarkdownEditor4::formatShiftLeft()
{
    QTextCursor c = textCursor();
    if (!c.hasSelection())
    {
        c.movePosition(QTextCursor::StartOfLine);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString text = c.selectedText();
        if (text.startsWith("  "))
        {
            text.remove(0, 2);
            c.insertText(text);
        }
    }
    else
    {
        auto start = c.selectionStart();
        auto end   = c.selectionEnd();
        c.setPosition(end);
        auto endLine = currentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; currentLineNumber(&c) <= endLine; index++)
        {
            c.movePosition(QTextCursor::EndOfLine);
            c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString text = c.selectedText();
            if (text.startsWith("  "))
            {
                text.remove(0, 2);
                c.insertText(text);
            }
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::Down);
        }
    }
    setTextCursor(c);
}

bool MarkdownEditor4::undoFormatting(const QString &formatter)
{
    QTextCursor c               = textCursor();
    QString     selectedText    = c.selectedText();
    int         formatterLength = formatter.length();
    int         selectionStart  = c.selectionStart();
    int         selectionEnd    = c.selectionEnd();

    c.setPosition(selectionStart - formatterLength);
    c.setPosition(selectionEnd + formatterLength, QTextCursor::KeepAnchor);
    QString selectedTextWithFormatter = c.selectedText();

    // if the formatter characters were found we remove them
    if (selectedTextWithFormatter.startsWith(formatter) && selectedTextWithFormatter.endsWith(formatter))
    {
        c.insertText(selectedText);
        return true;
    }

    return false;
}

void MarkdownEditor4::applyFormatter(const QString &formatter)
{
    QTextCursor c = textCursor();

    if (!c.hasSelection())
    {
        c.insertText(formatter.repeated(2));
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, formatter.length());
    }
    else
    {
        // first try to undo an existing formatting
        if (undoFormatting(formatter))
        {
            return;
        }
        QString                 selectedText = c.selectedText();
        QRegularExpressionMatch match        = QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch())
        {
            c.insertText(match.captured(1) + formatter + match.captured(2) + formatter + match.captured(3));
        }
    }
    setTextCursor(c);
}

int MarkdownEditor4::currentLineNumber(QTextCursor *cursor)
{
    QTextDocument *doc  = document();
    QTextBlock     blk  = doc->findBlock(cursor->position());
    QTextBlock     blk2 = doc->begin();

    int i = 1;
    while (blk != blk2)
    {
        blk2 = blk2.next();
        i++;
    }

    return i;
}

void MarkdownEditor4::replaceCurrentLineText(const QString &text)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.insertText(text);
}

void MarkdownEditor4::formatHeading(const QString &heading)
{
    QTextCursor c = textCursor();
    if (!c.hasSelection())
    {
        c.movePosition(QTextCursor::StartOfLine);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        c.insertText(heading + c.selectedText());
    }
    else
    {
        auto start = c.selectionStart();
        auto end   = c.selectionEnd();
        c.setPosition(end);
        auto endLine = currentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; currentLineNumber(&c) <= endLine; index++)
        {
            c.movePosition(QTextCursor::EndOfLine);
            c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString text = c.selectedText();
            c.insertText(heading + text);
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::Down);
        }
    }
    setTextCursor(c);
}

QTextCursor MarkdownEditor4::textCursor() const
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    return editor->textCursor();
}

void MarkdownEditor4::setTextCursor(const QTextCursor &cursor)
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    editor->setTextCursor(cursor);
}
