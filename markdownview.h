#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>
#include "rendereddocument.h"

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QResizeEvent);
class MarkdownEditor2;

class MarkdownView : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownView(QWidget *parent = nullptr);
    void forceConvert();
    bool maybeSave();
    void setThemeStyle();
    void updateMarkdownEngine();
    
signals:
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
    
public slots:
    void openDocument();
    void saveDocument();
    void saveAsDocument();
    void newDocument();
    void copy();
    void cut();
    void paste();
    void selectAll();
    void undo();
    void redo();
    void copyAsHTML();
    void exportAsHTML();
    void exportAsPDF();

private slots:
    void documentModified();
    void convertTimeout();
    void previewLoadFinished(bool);
    void pdfPrintingFinished(const QString &filePath, bool success);

protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    bool m_modified{false};
    QSplitter *m_splitter;
    MarkdownEditor2 *m_editor;
    QWebEngineView *m_preview;
    QTimer *m_convertTimer;
    QString m_savePath;
    RenderedDocument m_renderedContent;
    RenderedDocument m_themeStyle;
    
    void saveToFile(const QString &savePath);
    void renderMarkdownToHTML();
    void setContent(const QString &html);
};

#endif // MARKDOWNVIEW_H
