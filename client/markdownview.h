#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>

#include "rendereddocument.h"

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QResizeEvent);
QT_FORWARD_DECLARE_CLASS(QWebEnginePage);
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager);
class MarkdownEditor4;
class PreviewThemeEditor;
class PlantUMLUrlCodec;
class FileCache;

class MarkdownView : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownView(QNetworkAccessManager *nam, FileCache *fileCache, QWidget *parent = nullptr);
    ~MarkdownView();
    void             forceConvert();
    bool             maybeSave();
    void             updatePreviewTheme();
    void             updatePreviewMode();
    void             updateMarkdownEngine();
    void             updateMacStyleCodeBlock();
    void             openFromFile(const QString &fileName);
    void             setInitialDocument(const QString &content);
    MarkdownEditor4 *editor();
    QString          selectedText() const;
    QString          fullText() const;
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
    void contentModified();

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
    void onEmbedRenderingDone();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    bool                   m_modified {false};
    QSplitter *            m_splitter;
    MarkdownEditor4 *      m_editor;
    QWebEngineView *       m_preview;
    QTimer *               m_convertTimer;
    QNetworkAccessManager *m_nam;
    PreviewThemeEditor *   m_previewHTMLEditor;
    PreviewThemeEditor *   m_customPreivewThemeEditor;
    PlantUMLUrlCodec *     m_plantUMLUrlCodec {nullptr};
    FileCache *            m_fileCache;
    QString                m_savePath;
    RenderedDocument       m_renderedContent;
    RenderedDocument       m_themeStyle;
    RenderedDocument       m_wxboxWidth;
    RenderedDocument       m_macStyleCodeBlock;

    void saveToFile(const QString &savePath);
    void renderMarkdownToHTML();
    void setRenderedHTML(const QString &html);
};

#endif // MARKDOWNVIEW_H
