#include <vtextedit/vtextedit.h>

#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QtCore>

#include "markdowneditor4.h"
#include "clientutils.h"
#include "settings.h"

MarkdownEditor4::MarkdownEditor4(QWidget *parent)
    : vte::VMarkdownEditor(g_settings->markdownEditorConfig(), g_settings->textEditorParameters(), parent)
{
    setupChildWidgets();
}

MarkdownEditor4::MarkdownEditor4(const QSharedPointer<vte::MarkdownEditorConfig> &config,
                                 const QSharedPointer<vte::TextEditorParameters> &param,
                                 QWidget *                                        parent)
    : vte::VMarkdownEditor(config, param, parent)
{
    setupChildWidgets();
}

void MarkdownEditor4::setupChildWidgets()
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
    ClientUtils::InitializeWidgetFont(static_cast<QWidget *>(editor));
#if defined(Q_OS_MAC)
    zoom(4);
#endif
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

void MarkdownEditor4::emptyUndoBuffer()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto doc = editor->document();
    Q_ASSERT(doc);
    doc->clearUndoRedoStacks();
}

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
    c.beginEditBlock();
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
    c.endEditBlock();
    setTextCursor(c);
}

void MarkdownEditor4::formatComment()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
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
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void MarkdownEditor4::formatOrderedList()
{
    QTextCursor c = textCursor();
    c.beginEditBlock();
    if (!c.hasSelection())
    {
        c.insertText("1. ");
    }
    else
    {
        auto start = c.selectionStart();
        auto end   = c.selectionEnd();
        c.setPosition(end);
        auto endLine = cursorDocumentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; cursorDocumentLineNumber(&c) <= endLine; index++)
        {
            c.movePosition(QTextCursor::EndOfLine);
            c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString text = c.selectedText();
            c.insertText(QString("%1. %2").arg(index).arg(text));
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::Down);
        }
        auto curPos = c.position();
        c.setPosition(start, QTextCursor::MoveAnchor);
        c.setPosition(curPos - 1, QTextCursor::KeepAnchor);
    }
    c.endEditBlock();
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
    c.beginEditBlock();
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
            auto clipText = QApplication::clipboard()->text();
            if (!clipText.startsWith("http://") && !clipText.startsWith("https://"))
                clipText.clear();
            auto text = QString("%1[%2](%4)%3").arg(match.captured(1), match.captured(2), match.captured(3), clipText);
            c.insertText(text);
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    c.endEditBlock();
    setTextCursor(c);
}

void MarkdownEditor4::formatImage()
{
    QTextCursor c = textCursor();
    c.beginEditBlock();
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
            auto clipText = QApplication::clipboard()->text();
            if (!clipText.startsWith("http://") && !clipText.startsWith("https://"))
                clipText.clear();
            auto text = QString("%1![%2](%4)%3").arg(match.captured(1), match.captured(2), match.captured(3), clipText);
            c.insertText(text);
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        }
    }
    c.endEditBlock();
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
    c.beginEditBlock();
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
        auto endLine = cursorDocumentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; cursorDocumentLineNumber(&c) <= endLine; index++)
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

        auto curPos = c.position();
        c.setPosition(start, QTextCursor::MoveAnchor);
        c.setPosition(curPos - 1, QTextCursor::KeepAnchor);
    }
    c.endEditBlock();
    setTextCursor(c);
}

bool MarkdownEditor4::undoFormatting(const QString &formatter)
{
    QTextCursor c               = textCursor();
    c.beginEditBlock();
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
        c.endEditBlock();
        return true;
    }

    c.endEditBlock();
    return false;
}

void MarkdownEditor4::applyFormatter(const QString &formatter)
{
    QTextCursor c = textCursor();
    c.beginEditBlock();
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
            c.endEditBlock();
            return;
        }
        QString                 selectedText = c.selectedText();
        QRegularExpressionMatch match        = QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch())
        {
            c.insertText(match.captured(1) + formatter + match.captured(2) + formatter + match.captured(3));
        }
    }
    c.endEditBlock();
    setTextCursor(c);
}

int MarkdownEditor4::cursorDocumentLineNumber(QTextCursor *cursor)
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

int MarkdownEditor4::cursorEditorLineNumber(QTextCursor *cursor)
{
    const QTextBlock block = cursor->block();
    if (!block.isValid())
        return -1;

    const QTextLayout *layout = block.layout();
    if (!layout)
        return -1;
#if !defined(QT_NO_DEBUG)
    qDebug() << "cursor position:" << cursor->position() << ", block position:" << block.position();
#endif
    int  relativePos = cursor->position() - block.position();
    auto textLine    = layout->lineForTextPosition(relativePos);

    return textLine.lineNumber();
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
    c.beginEditBlock();
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
        auto endLine = cursorDocumentLineNumber(&c);
        c.setPosition(start);

        for (int index = 1; cursorDocumentLineNumber(&c) <= endLine; index++)
        {
            c.movePosition(QTextCursor::EndOfLine);
            c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString text = c.selectedText();
            if (!text.isEmpty())
                c.insertText(heading + text);
            c.movePosition(QTextCursor::StartOfLine);
            c.movePosition(QTextCursor::Down);
        }
        auto curPos = c.position();
        c.setPosition(start, QTextCursor::MoveAnchor);
        c.setPosition(curPos - 1, QTextCursor::KeepAnchor);
    }
    c.endEditBlock();
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

int MarkdownEditor4::currentDocumentLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto cursor = editor->textCursor();
    return cursorDocumentLineNumber(&cursor);
}

int MarkdownEditor4::currentEditorLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto cursor = editor->textCursor();
    return cursorEditorLineNumber(&cursor);
}

int MarkdownEditor4::documentLineCount()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto doc = editor->document();
    Q_ASSERT(doc);
    return doc->blockCount();
}

int MarkdownEditor4::editorLineCount()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto doc = editor->document();
    Q_ASSERT(doc);
    int blockCount = doc->blockCount();
    int lineCount  = 0;
    for (int i = 0; i < blockCount; i++)
    {
        auto block = doc->findBlockByNumber(i);
        lineCount += block.lineCount();
    }
    return lineCount;
}

int MarkdownEditor4::visibleLineCount()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto doc = editor->document();
    Q_ASSERT(doc);
    auto margin = doc->documentMargin();
    return (doc->size().height() - 2 * margin) / editor->fontMetrics().height();
}

int MarkdownEditor4::firstVisibleDocumentLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto cursor = editor->cursorForPosition(QPoint(0, 0));
    return cursorDocumentLineNumber(&cursor);
}

int MarkdownEditor4::firstVisibleEditorLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    auto cursor = editor->cursorForPosition(QPoint(0, 0));
    return cursorEditorLineNumber(&cursor);
}

int MarkdownEditor4::lastVisibleDocumentLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    QPoint ptBottomRight(editor->viewport()->width() - 1, editor->viewport()->height() - 1);
    auto   cursor = editor->cursorForPosition(ptBottomRight);
    return cursorDocumentLineNumber(&cursor);
}

int MarkdownEditor4::lastVisibleEditorLineNumber()
{
    vte::VTextEdit *editor = getTextEdit();
    Q_ASSERT(editor);
    QPoint ptBottomRight(editor->viewport()->width() - 1, editor->viewport()->height() - 1);
    auto   cursor = editor->cursorForPosition(ptBottomRight);
    return cursorEditorLineNumber(&cursor);
}
