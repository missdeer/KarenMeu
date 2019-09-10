#ifndef MARKDOWNEDITOR2_H
#define MARKDOWNEDITOR2_H

#include "qmarkdowntextedit.h"

class MarkdownEditor2 : public QMarkdownTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor2(QWidget *parent = nullptr);
    void initialize();
    void setContent(const QString& content);
    void setContent(const QByteArray& content);
    QByteArray content();
    bool modify();
    void setSavePoint();
    void emptyUndoBuffer();
    void updateCodeEditorFont();
    
signals:
    void contentModified();
    
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
    
private:
    
    /**
     * Attempts to undo the formatting on a selected string
     *
     * @param formatter
     * @return
     */
    bool undoFormatting(const QString& formatter);
    
    /**
     * Applies a formatter to a selected string
     *
     * @param formatter
     */
    void applyFormatter(const QString& formatter);
    
    int currentLineNumber();
    
    void replaceCurrentLineText(const QString& text);
    
    void formatHeading(const QString& heading);
    
};

#endif // MARKDOWNEDITOR2_H
