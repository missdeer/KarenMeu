#include <QRegularExpression>
#include "markdowneditor.h"

class UndoActionGuard
{
    ScintillaEdit* m_sci;
public:
    UndoActionGuard(ScintillaEdit* sci) : m_sci(sci)
    {m_sci->beginUndoAction();}
    ~UndoActionGuard() 
    {m_sci->endUndoAction();}
};


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
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert **|**
        auto pos = currentPos();
        insertText(pos, "****");
        setCurrentPos(pos+2);
        setSelectionStart(pos+2);
        setSelectionEnd(pos+2);
    }
    else 
    {
        // replace selected text with **selected text**
        auto startPos = selectionStart();
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("**"+text+"**");
        setSelectionStart(startPos+2);
        setSelectionEnd(endPos+2);
    }
}

void MarkdownEditor::formatEmphasize()
{
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert *|*
        auto pos = currentPos();
        insertText(pos, "**");
        setCurrentPos(pos+1);
        setSelectionStart(pos+1);
        setSelectionEnd(pos+1);
    }
    else 
    {
        // replace selected text with *selected text*
        auto startPos = selectionStart();
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("*"+text+"*");
        setSelectionStart(startPos+1);
        setSelectionEnd(endPos+1);
    }
}

void MarkdownEditor::formatInlineCode()
{
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert `|`
        auto pos = currentPos();
        insertText(pos, "``");
        setCurrentPos(pos+1);
        setSelectionStart(pos+1);
        setSelectionEnd(pos+1);
    }
    else 
    {
        // replace selected text with `selected text`
        auto startPos = selectionStart();
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("`"+text+"`");
        setSelectionStart(startPos+1);
        setSelectionEnd(endPos+1);
    }
}

void MarkdownEditor::formatComment()
{
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert <!--|-->
        auto pos = currentPos();
        insertText(pos, "<!---->");
        setCurrentPos(pos+4);
        setSelectionStart(pos+4);
        setSelectionEnd(pos+4);
    }
    else 
    {
        // replace selected text with <!--selected text-->
        auto startPos = selectionStart();
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("<!--"+text+"-->");
        setSelectionStart(startPos+4);
        setSelectionEnd(endPos+4);
    }
}

void MarkdownEditor::formatOrderedList()
{
    UndoActionGuard uag(this);
    auto startPos = selectionStart();
    auto endPos = selectionEnd();
    auto startLine = lineFromPosition(startPos);
    auto endLine = lineFromPosition(endPos);
    for (auto line = endLine; line >= startLine; line--)
    {
        auto lineText = getLine(line).trimmed();
        auto lineStartPos = positionFromLine(line);
        auto lineEndPos = lineEndPosition(line);
        lineText = QString("%1. ").arg(line-startLine + 1).toUtf8() + lineText;
        setTargetRange(lineStartPos, lineEndPos);
        replaceTarget(lineText.size(), lineText);
    }
    setSelectionStart(positionFromLine(startLine));
    setSelectionEnd(lineEndPosition(endLine));
    setCurrentPos(lineEndPosition(endLine));
}

void MarkdownEditor::formatUnorderedList()
{
    addLeadingString("- ");
}

void MarkdownEditor::formatBlockquote()
{
    addLeadingString("> ");
}

void MarkdownEditor::formatHyperlink()
{
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert [|]()
        auto pos = currentPos();
        insertText(pos, "[]()");
        setCurrentPos(pos+1);
        setSelectionStart(pos+1);
        setSelectionEnd(pos+1);
    }
    else 
    {
        // replace selected text with [selected text](|)
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("["+text+"]()");
        setSelectionStart(endPos+3);
        setSelectionEnd(endPos+3);
        setCurrentPos(endPos+3);
    }
}

void MarkdownEditor::formatImage()
{
    UndoActionGuard uag(this);
    if (selectionEmpty())
    {
        // insert ![|]()
        auto pos = currentPos();
        insertText(pos, "![]()");
        setCurrentPos(pos+2);
        setSelectionStart(pos+2);
        setSelectionEnd(pos+2);
    }
    else 
    {
        // replace selected text with ![selected text](|)
        auto endPos = selectionEnd();
        auto text = getSelText();
        replaceSel("!["+text+"]()");
        setSelectionStart(endPos+4);
        setSelectionEnd(endPos+4);
        setCurrentPos(endPos+4);
    }
}

void MarkdownEditor::formatNewParagraph()
{
    newLine();
}

void MarkdownEditor::formatHorizontalRule()
{
    insertText(currentPos(), "---");
}

void MarkdownEditor::formatHeader1()
{
    formatHeader(1);
}

void MarkdownEditor::formatHeader2()
{
    formatHeader(2);
}

void MarkdownEditor::formatHeader3()
{
    formatHeader(3);
}

void MarkdownEditor::formatHeader4()
{
    formatHeader(4);
}

void MarkdownEditor::formatHeader5()
{
    formatHeader(5);
}

void MarkdownEditor::formatHeader6()
{
    formatHeader(6);
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

void MarkdownEditor::formatHeader(int level)
{
    QByteArray leadingStr = QString(level, '#').toUtf8() + " ";
    UndoActionGuard uag(this);
    auto startPos = selectionStart();
    auto endPos = selectionEnd();
    auto startLine = lineFromPosition(startPos);
    auto endLine = lineFromPosition(endPos);
    QRegularExpression re("^#+\\s?");
    for (auto line = endLine; line >= startLine; line--)
    {
        auto lineText = getLine(line).trimmed();
        auto lineStartPos = positionFromLine(line);
        auto lineEndPos = lineEndPosition(line);
        QRegularExpressionMatch match = re.match(QString(lineText));
        if (match.hasMatch())
        {
            QString matched = match.captured(0);
            lineText.remove(0, matched.length());
        }
        lineText = leadingStr + lineText;
        setTargetRange(lineStartPos, lineEndPos);
        replaceTarget(lineText.size(), lineText);
    }
    setSelectionStart(positionFromLine(startLine));
    setSelectionEnd(lineEndPosition(endLine));
    setCurrentPos(lineEndPosition(endLine));
}

void MarkdownEditor::addLeadingString(const QByteArray &leadingStr)
{
    UndoActionGuard uag(this);
    auto startPos = selectionStart();
    auto endPos = selectionEnd();
    auto startLine = lineFromPosition(startPos);
    auto endLine = lineFromPosition(endPos);
    for (auto line = endLine; line >= startLine; line--)
    {
        auto lineText = getLine(line).trimmed();
        auto lineStartPos = positionFromLine(line);
        auto lineEndPos = lineEndPosition(line);
        lineText = leadingStr + lineText;
        setTargetRange(lineStartPos, lineEndPos);
        replaceTarget(lineText.size(), lineText);
    }
    setSelectionStart(positionFromLine(startLine));
    setSelectionEnd(lineEndPosition(endLine));
    setCurrentPos(lineEndPosition(endLine));
}
