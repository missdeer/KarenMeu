#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>

#include "rendereddocument.h"

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QResizeEvent);
QT_FORWARD_DECLARE_CLASS(QWebEnginePage);
class MarkdownEditor3;
class PreviewThemeEditor;

class MarkdownView : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownView(QWidget *parent = nullptr);
    ~MarkdownView();
    void             forceConvert();
    bool             maybeSave();
    void             updatePreviewTheme();
    void             updatePreviewMode();
    void             updateMarkdownEngine();
    void             updateMacStyleCodeBlock();
    void             openFromFile(const QString &fileName);
    MarkdownEditor3 *editor();
    QString          selectedText() const;
    QSplitter *      splitter();
    void             setPreviewHTMLEditor(PreviewThemeEditor *previewHTMLEditor);
    void             setCustomPreivewThemeEditor(PreviewThemeEditor *customPreivewThemeEditor);
    QWebEnginePage * devToolPage();
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
    void setCurrentFile(const QString &);
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
    bool                m_modified {false};
    QSplitter *         m_splitter;
    MarkdownEditor3 *   m_editor;
    QWebEngineView *    m_preview;
    QTimer *            m_convertTimer;
    PreviewThemeEditor *m_previewHTMLEditor;
    PreviewThemeEditor *m_customPreivewThemeEditor;
    QString             m_savePath;
    RenderedDocument    m_renderedContent;
    RenderedDocument    m_themeStyle;
    RenderedDocument    m_wxboxWidth;
    RenderedDocument    m_macStyleCodeBlock;

    void saveToFile(const QString &savePath);
    void renderMarkdownToHTML();
    void setContent(const QString &html);
};

#endif // MARKDOWNVIEW_H
