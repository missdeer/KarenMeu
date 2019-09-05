#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "ScintillaEdit.h"
#include "scintillaconfig.h"

class MarkdownEditor : public ScintillaEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    void initialize();
    void setContent(const QString& content);
    void setContent(const QByteArray& content);
    QByteArray content();
signals:
    void contentModified();
    
public slots:
    void formatStrong();
    void formatEmphasize();
    void formatInlineCode();
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
    
private slots:
    void linesAdded(int linesAdded);
    void marginClicked(int position, int modifiers, int margin);
    void modified(int type, int position, int length, int linesAdded,
                  const QByteArray &text, int line, int foldNow, int foldPrev);
    
private:
    ScintillaConfig m_sc;
};

#endif // MARKDOWNEDITOR_H
