#include <QCommonStyle>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextDocumentFragment>
#include <QtCore>

#include "markdowneditor3.h"

#include "settings.h"

MarkdownEditor3::MarkdownEditor3(QWidget *parent) : QMarkdownTextEdit(parent)
{
    connect(this, &QPlainTextEdit::modificationChanged, [this](bool changed) {
        if (changed)
            emit contentModified();
    });
    connect(this, &QPlainTextEdit::textChanged, this, &MarkdownEditor3::contentModified);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, [this](int) {
        emit scrollValueChanged(verticalScrollBar()->value(), verticalScrollBar()->maximum());
    });
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, [this](int, int) {
        emit scrollValueChanged(verticalScrollBar()->value(), verticalScrollBar()->maximum());
    });
}

void MarkdownEditor3::updateCodeEditorFont()
{
    QFont font(this->font());
    font.setWeight(QFont::Normal);
    font.setItalic(false);
    font.setPointSizeF(g_settings->codeEditorFontPointSize());

    QStringList fonts;
#if defined(Q_OS_WIN)
    fonts << "Microsoft YaHei UI";
#elif defined(Q_OS_MAC)
    fonts << "PingFang SC"
          << "PingFang HK"
          << "PingFang TC";
#else
    fonts << "WenQuanYi Micro Hei";
#endif
    fonts << font.families();
    QFont::insertSubstitutions(font.family(), fonts);
    font.setFamilies(fonts);
    setFont(font);
}

void MarkdownEditor3::initialize()
{
    new MarkdownHighlighter(document());
    updateCodeEditorFont();
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
}

void MarkdownEditor3::setContent(const QString &content)
{
    setPlainText(content);
}

void MarkdownEditor3::setContent(const QByteArray &content)
{
    setPlainText(content);
}

QByteArray MarkdownEditor3::content()
{
    return toPlainText().toUtf8();
}

bool MarkdownEditor3::modify()
{
    return document()->isModified();
}

void MarkdownEditor3::setSavePoint()
{
    document()->setModified(false);
}

void MarkdownEditor3::emptyUndoBuffer() {}

void MarkdownEditor3::formatStrong()
{
    applyFormatter("**");
}

void MarkdownEditor3::formatEmphasize()
{
    applyFormatter("*");
}

void MarkdownEditor3::formatStrikethrough()
{
    applyFormatter("~~");
}

void MarkdownEditor3::formatInlineCode()
{
    applyFormatter("`");
}

void MarkdownEditor3::formatCodeBlock()
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

void MarkdownEditor3::formatComment()
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

void MarkdownEditor3::formatOrderedList()
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

void MarkdownEditor3::formatUnorderedList()
{
    formatHeading("* ");
}

void MarkdownEditor3::formatBlockquote()
{
    formatHeading("> ");
}

void MarkdownEditor3::formatHyperlink()
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

void MarkdownEditor3::formatImage()
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

void MarkdownEditor3::formatNewParagraph()
{
    QTextCursor c = textCursor();
    c.insertText("\n");

    setTextCursor(c);
}

void MarkdownEditor3::formatHorizontalRule()
{
    QTextCursor c = textCursor();
    c.insertText("---\n");

    setTextCursor(c);
}

void MarkdownEditor3::formatHeader1()
{
    formatHeading("# ");
}

void MarkdownEditor3::formatHeader2()
{
    formatHeading("## ");
}

void MarkdownEditor3::formatHeader3()
{
    formatHeading("### ");
}

void MarkdownEditor3::formatHeader4()
{
    formatHeading("#### ");
}

void MarkdownEditor3::formatHeader5()
{
    formatHeading("##### ");
}

void MarkdownEditor3::formatHeader6()
{
    formatHeading("###### ");
}

void MarkdownEditor3::formatShiftRight()
{
    formatHeading("  ");
}

void MarkdownEditor3::formatShiftLeft()
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

bool MarkdownEditor3::undoFormatting(const QString &formatter)
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

void MarkdownEditor3::applyFormatter(const QString &formatter)
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

int MarkdownEditor3::currentLineNumber(QTextCursor *cursor)
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

void MarkdownEditor3::replaceCurrentLineText(const QString &text)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.insertText(text);
}

void MarkdownEditor3::formatHeading(const QString &heading)
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
