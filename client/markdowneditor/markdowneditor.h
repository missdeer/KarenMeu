#ifndef MARKDOWNEDITOR4_H
#define MARKDOWNEDITOR4_H

#include <vtextedit/markdowneditorconfig.h>
#include <vtextedit/vmarkdowneditor.h>

class MarkdownEditor : public vte::VMarkdownEditor
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    explicit MarkdownEditor(const QSharedPointer<vte::MarkdownEditorConfig> &config,
                            const QSharedPointer<vte::TextEditorParameters> &param,
                            QWidget                                         *parent = nullptr);

    void       initialize();
    void       setContent(const QString &content);
    void       setContent(const QByteArray &content);
    QByteArray content();
    bool       modify();
    void       setSavePoint();
    void       emptyUndoBuffer();

    void clear();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void selectAll();

    [[nodiscard]] QTextCursor textCursor() const;
    void        setTextCursor(const QTextCursor &cursor);

    int currentDocumentLineNumber();
    int currentEditorLineNumber();
    int documentLineCount();
    int editorLineCount();
    int visibleLineCount();
    int firstVisibleDocumentLineNumber();
    int firstVisibleEditorLineNumber();
    int lastVisibleDocumentLineNumber();
    int lastVisibleEditorLineNumber();
signals:
    void contentModified();
    void scrollValueChanged(int, int);
public slots:
    void formatStrong();
    void formatEmphasize();
    void formatStrikethrough();
    void formatInlineCode();
    void formatCodeBlock();
    void formatComment();
    void formatOrderedList();
    void formatUnorderedList();
    void formatBlockquote();
    void formatHyperlink();
    void formatImage();
    void formatNewParagraph();
    void formatHorizontalRule();
    void formatHeader1();
    void formatHeader2();
    void formatHeader3();
    void formatHeader4();
    void formatHeader5();
    void formatHeader6();
    void formatShiftRight();
    void formatShiftLeft();
    void insertText(const QString &text);

  private:
    /**
     * Attempts to undo the formatting on a selected string
     *
     * @param formatter
     * @return
     */
    bool undoFormatting(const QString &formatter);

    /**
     * Applies a formatter to a selected string
     *
     * @param formatter
     */
    void applyFormatter(const QString &formatter);

    int cursorDocumentLineNumber(QTextCursor *cursor);
    int cursorEditorLineNumber(QTextCursor *cursor);

    void replaceCurrentLineText(const QString &text);

    void formatHeading(const QString &heading);
    void setupChildWidgets();
};

#endif // MARKDOWNEDITOR4_H
